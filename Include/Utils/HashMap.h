#pragma once
#include <concepts>
#include <cstdint>
#include <mimalloc.h>
#include<vector>
#include "Array.h"
#include "Hash.h"

#include <array>
#include <limits>
#include <new>

#ifdef EUGENE_SSE2
#include <emmintrin.h>
#endif

namespace Eugene
{

	template<class Key, std::unsigned_integral HashValueType>
	class HashCalcFunc
	{
	public:
		using ReturnValueType = HashValueType;
		ReturnValueType operator()(const Key& key) const
		{
		// Use std::hash to avoid overload ambiguity with FNV implementations.
		if constexpr (std::is_same<HashValueType, std::uint64_t>::value)
		{
			return static_cast<ReturnValueType>(std::hash<Key>{}(key));
		}
		else
		{
			return static_cast<ReturnValueType>(std::hash<Key>{}(key) & 0xFFFFFFFFu);
		}
		}
	};

	template<class Key, std::unsigned_integral HashValueType>
	class HashNoCalcHashFunc
	{
	public:
		using ReturnValueType = HashValueType;
		constexpr ReturnValueType operator()(const Key& key)  const noexcept
		{
			return key;
		}
	};

	/// <summary>
	/// 固定サイズの軽量なハッシュマップ（キー衝突を想定していないので注意）
	/// </summary>
	/// <typeparam name="IndexSizeType"> インデックスの型 </typeparam>
	/// <typeparam name="Value"> 値の型 </typeparam>
	/// <typeparam name="HashFunc"> ハッシュ関数の型 </typeparam>
	/// <typeparam name="Key"> キーの型 </typeparam>
	template<class Key,class Value, std::unsigned_integral IndexSizeType = std::uint32_t, class HashFunc = HashCalcFunc<Key, std::uint64_t>>
	class FixedHashMap
	{
	public:
		struct iterator
		{
			iterator() :
				map_{ nullptr }, index_{ 0 }
			{
			}
			iterator(FixedHashMap* map, const IndexSizeType index) :
				map_{ map }, index_{ index }
			{
			}
			Value& operator*()
			{
				return map_->dataTable_[index_];
			}
			iterator& operator++()
			{
				index_++;
				return *this;
			}
			bool operator==(const iterator& itr)
			{
				return index_ == itr.index_;
			}
			bool operator!=(const iterator& itr)
			{
				return index_ != itr.index_;
			}
		private:
			FixedHashMap* map_;
			IndexSizeType index_;
		};

		struct const_iterator
		{
			const_iterator() :
				map_{ nullptr }, index_{ 0 }
			{
			}
			const_iterator(const FixedHashMap* map, const IndexSizeType index) :
				map_{ map }, index_{ index }
			{
			}
			const Value& operator*() const
			{
				return map_->dataTable_[index_];
			}
			const_iterator& operator++()
			{
				index_++;
				return *this;
			}
			bool operator==(const const_iterator& itr)
			{
				return index_ == itr.index_;
			}
			bool operator!=(const const_iterator& itr)
			{
				return index_ != itr.index_;
			}
		private:
			const FixedHashMap* map_;
			IndexSizeType index_;
		};

		FixedHashMap() :
			mask{ 0 }
		{
		}

		FixedHashMap(const IndexSizeType size) :
			dataTable_{ size }, mask{ size - 1 }
		{
		}

		void Init(const IndexSizeType size)
		{
			dataTable_.Resize(size);
			mask = size - 1;
		}

		bool Contains(const IndexSizeType key) const
		{
			auto index = Key2Index(key);
			return index >= dataTable_.size();
		}

		Value& operator[](const Key& key)
		{
			auto index = Key2Index(key);
			return dataTable_[index];
		}

		const Value& operator[](const Key& key) const
		{
			auto index = Key2Index(key);
			return dataTable_[index];
		}

		iterator begin()
		{
			return { this, 0 };
		}

		iterator end()
		{
			return { this, static_cast<IndexSizeType>(dataTable_.size()) };
		}

		const_iterator begin() const
		{
			return { this, 0 };
		}

		const_iterator end() const
		{
			return { this, static_cast<IndexSizeType>(dataTable_.size()) };
		}

		const_iterator cbegin() const
		{
			return { this, 0 };
		}

		const_iterator cend() const
		{
			return { this, static_cast<IndexSizeType>(dataTable_.size()) };
		}
	private:
		IndexSizeType Key2Index(const Key& key) const
		{
			return HashFunc{}(key) & mask;
		}

		Array<Value, IndexSizeType> dataTable_;   // データテーブル
		IndexSizeType mask = 0;			// table_size - 1
	};


	/// <summary>
	/// SwissTableっぽいもので作ったハッシュマップ
	/// </summary>
	/// <typeparam name="Key"> キーの型 </typeparam>
	/// <typeparam name="Value"> 値の型 </typeparam>
	/// <typeparam name="HashFunc"> ハッシュ関数の型 </typeparam>
	/// <typeparam name="EqualFunc"> 等価比較関数の型 </typeparam>
	/// <typeparam name="SizeType"> サイズの型 </typeparam>
	/// <typeparam name="rehashLoadFactor"> リハッシュが行われる使用率 </typeparam>
	/// <typeparam name="rehashSizeScale"> リハッシュが行われる際の倍率 </typeparam>
	template<
		class Key, class Value, std::unsigned_integral SizeType = std::uint32_t,
		class HashFunc = HashCalcFunc<Key, std::uint64_t>, 
		class EqualFunc = std::equal_to<Key>,
		float rehashLoadFactor = 0.8f,
		SizeType rehashSizeScale = 2
	>
	class SwissTable
	{
	public:
		SwissTable(const SizeType groupSize = 16) noexcept :
			groupSize_{ 0 }, mask{ 0 }, ctrl_{ nullptr }, keys_{ nullptr }, values_{ nullptr }, size_{ 0 }
		{
			Rehash(groupSize);
		}

		SwissTable(const SwissTable& table) noexcept :
			SwissTable{ table.groupSize_ }
		{
			for (SizeType i = 0; i < groupSize_; i++)
			{
				for (SizeType j = 0; j < static_cast<SizeType>(ctrl_[i].size()); j++)
				{
					if (table.ctrl_[i][j] != EMPTY && table.ctrl_[i][j] != TOMBSTONE)
					{
						ctrl_[i][j] = table.ctrl_[i][j];
						new(&keys_[i][j]) Key{ table.keys_[i][j] };
						new(&values_[i][j]) Value{ table.values_[i][j] };
						size_++;
					}
				}
			}
		}

		SwissTable(SwissTable&& table) noexcept :
			keys_{ table.keys_ }, values_{ table.values_ }, ctrl_{ table.ctrl_ },
			groupSize_{ table.groupSize_ }, mask{ table.mask }, size_{ table.size_ }
		{
			table.keys_ = nullptr;
			table.values_ = nullptr;
			table.ctrl_ = nullptr;
			table.groupSize_ = 0;
			table.mask = 0;
			table.size_ = 0;
		}

		~SwissTable() noexcept
		{
			if (keys_ != nullptr)
			{
				for (SizeType i = 0; i < groupSize_; i++)
				{
					for (SizeType j = 0; j < ctrlArraySize; j++)
					{
						if (ctrl_[i][j] != EMPTY && ctrl_[i][j] != TOMBSTONE)
						{
							keys_[i][j].~Key();
							values_[i][j].~Value();
						}
					}
				}
#ifndef EUGENE_TEST
				mi_free(keys_);
				mi_free(values_);
				mi_free(ctrl_);
#else
				std::free(keys_);
				std::free(values_);
				std::free(ctrl_);
#endif
			}
		}

		struct iterator
		{
			iterator() noexcept :
				map_{ nullptr }, groupIndex_{ 0 }, slotIndex_{ 0 }
			{
			}
			iterator(SwissTable* map, const SizeType groupIndex, const SizeType slotIndex) noexcept :
				map_{ map }, groupIndex_{ groupIndex }, slotIndex_{ slotIndex }
			{
			}

			Value& operator*() noexcept
			{
				return map_->values_[groupIndex_][slotIndex_];
			}

			~iterator() noexcept
			{
			}
			iterator& operator++() noexcept
			{
				slotIndex_++;
				if (slotIndex_ >= SwissTable::ctrlArraySize)
				{
					groupIndex_++;
					slotIndex_ = 0;
				}
				return *this;
			}
			bool operator==(const iterator& itr)
			{
				return groupIndex_ == itr.groupIndex_ && slotIndex_ == itr.slotIndex_;
			}
			bool operator!=(const iterator& itr)
			{
				return !(*this == itr);
			}
		private:
			SwissTable* map_;
			SizeType groupIndex_;
			SizeType slotIndex_;
		};

		struct const_iterator
		{
			const_iterator() :
				map_{ nullptr }, groupIndex_{ 0 }, slotIndex_{ 0 }
			{
			}
			const_iterator(const SwissTable* map, const SizeType groupIndex, const SizeType slotIndex) :
				map_{ map }, groupIndex_{ groupIndex }, slotIndex_{ slotIndex }
			{
			}
			const Value& operator*() const
			{
				return map_->values_[groupIndex_][slotIndex_];
			}

			const_iterator& operator++()
			{
				slotIndex_++;
				if (slotIndex_ >= SwissTable::ctrlArraySize)
				{
					groupIndex_++;
					slotIndex_ = 0;
				}
				return *this;
			}
			bool operator==(const const_iterator& itr)
			{
				return groupIndex_ == itr.groupIndex_ && slotIndex_ == itr.slotIndex_;
			}
			bool operator!=(const const_iterator& itr)
			{
				return !(*this == itr);
			}
		private:
			const SwissTable* map_;
			SizeType groupIndex_;
			SizeType slotIndex_;
		};

		Value& operator[](const Key& key) & noexcept
		{
			CheckRehash();
			SizeType rtnIdx = 0;
			SizeType rtnSlot = 0;
			if (InternalInsert(key, ctrl_, keys_, groupSize_, mask,
				[&](SizeType idx, SizeType slot)
				{
					rtnIdx = idx;
					rtnSlot = slot;
				},
				[&](SizeType idx, SizeType slot, std::uint8_t h2)
				{
					new(&keys_[idx][slot]) Key{ key };
					new(&values_[idx][slot]) Value{};
					ctrl_[idx][slot] = h2;
					size_++;
					rtnIdx = idx;
					rtnSlot = slot;
				}
			))
			{
				return values_[rtnIdx][rtnSlot];
			}
			EUGENE_ASSERT(false); // 空きスロットが見つからない場合は再ハッシュが必要
		}

		const Value& At(const Key& key) const & noexcept
		{
			CheckRehash();
			SizeType rtnIdx = 0;
			SizeType rtnSlot = 0;
			if (InternalInsert(key, ctrl_, keys_, groupSize_, mask,
				[&](SizeType idx, SizeType slot)
				{
					rtnIdx = idx;
					rtnSlot = slot;
				},
				[&](SizeType idx, SizeType slot, std::uint8_t h2)
				{
					new(&keys_[idx][slot]) Key{ key };
					new(&values_[idx][slot]) Value{};
					ctrl_[idx][slot] = h2;
					size_++;
					rtnIdx = idx;
					rtnSlot = slot;
				}
			))
			{
				return values_[rtnIdx][rtnSlot];
			}
			EUGENE_ASSERT(false); // 空きスロットが見つからない場合は再ハッシュが必要
		}

		bool Contains(const Key& key) const noexcept
		{
			return InternalFind(
				key, ctrl_, keys_, groupSize_, mask,
				[](SizeType idx, SizeType slot) {  }
			);
		}

		bool Insert(const Key& key, const Value& value) noexcept
		{
			CheckRehash();
			return (InternalInsert(key, ctrl_, keys_, groupSize_, mask,
				[&](SizeType idx, SizeType slot)
				{
					values_[idx][slot] = value;
				},
				[&](SizeType idx, SizeType slot, std::uint8_t h2)
				{
					new(&keys_[idx][slot]) Key{ key };
					new(&values_[idx][slot]) Value{ value };
					ctrl_[idx][slot] = h2;
					size_++;
				}
			));
		}

		bool Emplace(const Key& key, Value&& value) noexcept
		{
			CheckRehash();
			return (InternalInsert(key, ctrl_, keys_, groupSize_, mask,
				[&](SizeType idx, SizeType slot)
				{
					values_[idx][slot] = std::move(value);
				},
				[&](SizeType idx, SizeType slot, std::uint8_t h2)
				{
					new(&keys_[idx][slot]) Key{ std::move(key) };
					new(&values_[idx][slot]) Value{ std::move(value) };
					ctrl_[idx][slot] = h2;
					size_++;
				}
			));
		}

		void Erace(const Key& key) noexcept
		{
			InternalFind(
				key, ctrl_, keys_, groupSize_, mask,
				[this](SizeType idx, SizeType slot) {
					keys_[idx][slot].~Key();
					values_[idx][slot].~Value();
					ctrl_[idx][slot] = TOMBSTONE;
				}
			);
		}

		SizeType size() const noexcept { return size_; }

		iterator Find(const Key& key) & noexcept
		{
			SizeType rtnIdx = 0;
			SizeType rtnSlot = 0;
			if (InternalFind(
				key, ctrl_, keys_, groupSize_, mask,
				[&rtnIdx, &rtnSlot](SizeType idx, SizeType slot) { rtnIdx = idx; rtnSlot = slot; }
				))
			{
				return iterator{ this, rtnIdx, rtnSlot };
			}
			return end();
		}

		void Rehash(SizeType newGroupSize) noexcept
		{
#ifdef _DEBUG
			// Rehash called
#endif
#ifndef EUGENE_TEST
			auto newKeys = static_cast<KeyArrayType*>(mi_new_aligned_nothrow(sizeof(KeyArrayType) * newGroupSize, alignof(KeyArrayType)));
			auto newValues = static_cast<ValueArrayType*>(mi_new_aligned_nothrow(sizeof(ValueArrayType) * newGroupSize, alignof(ValueArrayType)));
			auto newCtrl = static_cast<CtrlArrayType*>(mi_new_aligned_nothrow(sizeof(CtrlArrayType) * newGroupSize, ctrlArraySize));
#else
			auto newKeys = static_cast<KeyArrayType*>(std::malloc(sizeof(KeyArrayType) * newGroupSize));
			auto newValues = static_cast<ValueArrayType*>(std::malloc(sizeof(ValueArrayType) * newGroupSize));
			auto newCtrl = static_cast<CtrlArrayType*>(std::malloc(sizeof(CtrlArrayType) * newGroupSize));
#endif
			// 明示的に割り当て失敗をチェックする。Release では EUGENE_ASSERT が無効化される可能性があるため
			if (newKeys == nullptr || newValues == nullptr || newCtrl == nullptr)
			{
				// 部分的に割り当てられているものがあれば解放
#ifndef EUGENE_TEST
				if (newKeys) mi_free(newKeys);
				if (newValues) mi_free(newValues);
				if (newCtrl) mi_free(newCtrl);
				if (keys_) mi_free(keys_);
				if (values_) mi_free(values_);
				if (ctrl_) mi_free(ctrl_);
#else
				if (newKeys) std::free(newKeys);
				if (newValues) std::free(newValues);
				if (newCtrl) std::free(newCtrl);
				if (keys_) std::free(keys_);
				if (values_) std::free(values_);
				if (ctrl_) std::free(ctrl_);
#endif
				EUGENE_RELEASE_ASSERT(false);
			}
			for (SizeType i = 0; i < newGroupSize; i++)
			{
				newCtrl[i].fill(EMPTY);
			}
			auto newMask = newGroupSize - 1;

			if (keys_ != nullptr)
			{
				size_ = 0;
				for (SizeType i = 0; i < groupSize_; ++i)
				{
					for (SizeType j = 0; j < ctrlArraySize; ++j)
					{
						(InternalInsert(keys_[i][j], newCtrl, newKeys, newGroupSize, newMask,
							[](SizeType idx, SizeType slot)
							{
								//values_[idx][slot] = value;
							},
							[&](SizeType idx, SizeType slot, std::uint8_t h2)
							{
								new(&newKeys[idx][slot]) Key{ std::move(keys_[i][j]) };
								new(&newValues[idx][slot]) Value{ std::move(values_[i][j]) };
								newCtrl[idx][slot] = h2;
								size_++;
							}
						));
					}
				}
#ifndef EUGENE_TEST
				mi_free(keys_);
				mi_free(values_);
				mi_free(ctrl_);
#else
				std::free(keys_);
				std::free(values_);
				std::free(ctrl_);
#endif
			}
			keys_ = newKeys;
			values_ = newValues;
			ctrl_ = newCtrl;
			mask = newGroupSize - 1;
			groupSize_ = newGroupSize;
		}

		iterator begin() noexcept
		{
			return iterator(this, 0, 0);
		}

		iterator end() noexcept
		{
			return iterator(this, groupSize_, ctrlArraySize);
		}

		const_iterator cbegin() const noexcept
		{
			return const_iterator(this, 0, 0);
		}

		const_iterator cend() const noexcept
		{
			return const_iterator(this, groupSize_, ctrlArraySize);
		}

	private:
#ifdef EUGENE_SSE2
		static constexpr SizeType ctrlArraySize = 16;
#else
		static constexpr SizeType ctrlArraySize = 8;
#endif
		using CtrlArrayType = std::array<std::uint8_t, ctrlArraySize>;
		using ValueArrayType = std::array<Value, ctrlArraySize>;
		using KeyArrayType = std::array<Key, ctrlArraySize>;

		// 8bit モード: 0 を EMPTY、0xFF を TOMBSTONE に予約する
		static constexpr uint8_t EMPTY = 0x00;
		static constexpr uint8_t TOMBSTONE = 0xFF;

		KeyArrayType* keys_;
		ValueArrayType* values_;
		CtrlArrayType* ctrl_;

		SizeType mask = 0;
		SizeType groupSize_;
		SizeType size_ = 0;

	private:
		/// <summary>
		/// チェックしてリハッシュが必要ならリハッシュする
		/// </summary>
		void CheckRehash() noexcept
		{
			SizeType capacity = groupSize_ * static_cast<SizeType>(ctrlArraySize);
			SizeType threshold = static_cast<SizeType>(static_cast<float>(capacity) * rehashLoadFactor);
			if (size_ >= threshold)
			{
				Rehash(groupSize_ * rehashSizeScale);
			}
		}

		template<std::unsigned_integral T>
		static constexpr uint8_t CalcH2FromHash(const T h) noexcept
		{
			// 下位8bit を取り出すが、0(EMPTY) と 0xFF(TOMBSTONE) は予約値なので別の値へマップする
			uint8_t raw = static_cast<uint8_t>(h & 0xFFu);
			if (raw == EMPTY) return static_cast<uint8_t>(1);
			if (raw == TOMBSTONE) return static_cast<uint8_t>(0xFE);
			return raw;
		}

		static SizeType SearchEmptyCtrl(const CtrlArrayType& ctrl) noexcept
		{
			for (SizeType i = 0; i < static_cast<SizeType>(ctrl.size()); i++)
			{
				if (ctrl[i] == EMPTY || ctrl[i] == TOMBSTONE)
				{
					return i;
				}
			}
			return std::numeric_limits<SizeType>::max();
		}

		// MatchH2 は 64bit のビットマスクを返す必要があるため MetaDataType を返す
		static std::uint64_t MatchH2(const std::array<std::uint8_t,8>& ctrl, uint8_t h2) noexcept
		{
			// 内部のビット演算は64bitで行う（MetaDataType）
			std::uint64_t x = 0;
			std::memcpy(&x, ctrl.data(), sizeof(x)); // 安全にロード（ctrlArraySize == sizeof(MetaDataType) のはず）

			// key を全バイトに展開
			std::uint64_t k = (0x0101010101010101ULL) * static_cast<std::uint64_t>(h2);

			std::uint64_t y = x ^ k; // 一致バイトは0になる

			// 各バイトが0かどうかを検出
			std::uint64_t tmp = (y - (0x0101010101010101ULL)) & ~y & (0x8080808080808080ULL);

			return tmp; // 各バイトのMSBにフラグが立つ（64bitマスクを返す）
		}

		// 修正: SearchEmpty の条件が逆になっている（空き or tombstone を見つけるべき）
		static std::pair<SizeType, SizeType> SearchEmpty(const CtrlArrayType* ctrl, const KeyArrayType* keys, SizeType groupSize) noexcept
		{
			for (SizeType i = 0; i < groupSize; i++)
			{
				for (SizeType j = 0; j < static_cast<SizeType>(ctrl[i].size()); j++)
				{
				// 修正: 空きスロットまたはトゥームストーンを検出する条件にする
					if (ctrl[i][j] == EMPTY || ctrl[i][j] == TOMBSTONE)
					{
						return { i, j };
					}
				}
			}
			return { std::numeric_limits<SizeType>::max(), std::numeric_limits<SizeType>::max() };
		}

		// 指定グループに EMPTY バイトが存在するか
		static bool GroupHasEmpty(const CtrlArrayType& ctrl) noexcept
		{
			std::uint64_t x = 0;
			std::memcpy(&x, ctrl.data(), sizeof(x)); // 安全にロード（ctrlArraySize == sizeof(MetaDataType) のはず）
			return (x - 0x0101010101010101ULL) & ~x & 0x8080808080808080ULL;
		}

		// 修正: FindH2 のループ内で候補ビットごとに slot を再計算する
		SizeType FindH2(const std::uint8_t h2, const Key& key, SizeType groupIndex) const noexcept
		{
			auto r64 = MatchH2(ctrl_[groupIndex], h2);
			// MatchH2 は MetaDataType（64bit）で計算しているので、反復は64bit 値で行う
			for (; r64 != 0; r64 &= (r64 - 1))
			{
				unsigned bit = std::countr_zero(r64); // 最下位セットビットのビット位置
				SizeType slot = static_cast<SizeType>(bit >> 3); // バイト単位 -> スロットインデックス
				if (EqualFunc{}(keys_[groupIndex][slot], key))
				{
					return slot;
				}
			}
			return std::numeric_limits<SizeType>::max(); // キーが見つからなかった
		}

		static SizeType FindH2(const std::uint8_t h2, const std::array<std::uint8_t, 8>& ctrl, const Key& key, const std::array<Key,8>& keys) noexcept
		{
			auto r64 = MatchH2(ctrl, h2);
			// MatchH2 は MetaDataType（64bit）で計算しているので、反復は64bit 値で行う
			for (; r64 != 0; r64 &= (r64 - 1))
			{
				unsigned bit = std::countr_zero(r64); // 最下位セットビットのビット位置
				SizeType slot = static_cast<SizeType>(bit >> 3); // バイト単位 -> スロットインデックス
				if (EqualFunc{}(keys[slot], key))
				{
					return slot;
				}
			}
			return std::numeric_limits<SizeType>::max(); // キーが見つからなかった
		}

		/// <summary>
		/// 8bit SIMD不使用版の走査処理
		/// </summary>
		/// <typeparam name="A"></typeparam>
		/// <param name="key"></param>
		/// <param name="ctrl"></param>
		/// <param name="keys"></param>
		/// <param name="groupSize"></param>
		/// <param name="mask"></param>
		/// <param name="contains"></param>
		/// <returns></returns>
		template<class A>
		static bool InternalFind(const Key& key, const std::array<std::uint8_t, 8>* ctrl, const std::array<Key, 8>* keys, SizeType groupSize, SizeType mask, A&& contains)
		{
			auto h = HashFunc{}(key);
			SizeType idx = h & mask;
			uint8_t h2 = CalcH2FromHash(h);
			for (SizeType i = idx; i < groupSize; i++)
			{
				SizeType result = FindH2(h2, ctrl[i], key, keys[i]);
				if (result != std::numeric_limits<SizeType>::max())
				{
					contains(i, result);
					return true;
				}
			}
			for (SizeType i = 0; i < idx; i++)
			{
				SizeType result = FindH2(h2, ctrl[i], key, keys[i]);
				if (result != std::numeric_limits<SizeType>::max())
				{
					contains(i, result);
					return true;
				}
			}
			return false;
		}

		/// <summary>
		/// 8bit SIMD不使用版の挿入用の走査処理
		/// </summary>
		/// <typeparam name="A"></typeparam>
		/// <typeparam name="B"></typeparam>
		/// <param name="key"></param>
		/// <param name="ctrl"></param>
		/// <param name="keys"></param>
		/// <param name="groupSize"></param>
		/// <param name="mask"></param>
		/// <param name="contains"></param>
		/// <param name="empty"></param>
		/// <returns></returns>
		template<class A, class B>
		static bool InternalInsert(const Key& key, const std::array<std::uint8_t, 8>* ctrl, const std::array<Key, 8>* keys, SizeType groupSize, SizeType mask, A&& contains, B&& empty)
		{
			auto h = HashFunc{}(key);
			SizeType idx = h & mask;
			uint8_t h2 = CalcH2FromHash(h);

			// Probe groups incrementally from home slot, stop when encountering an EMPTY in a group
			SizeType firstTombGroup = std::numeric_limits<SizeType>::max();
			SizeType firstTombSlot = std::numeric_limits<SizeType>::max();
			for (SizeType probe = 0; probe < groupSize; ++probe)
			{
				SizeType i = (idx + probe) & mask;
				// check for existing key in this group
				SizeType found = FindH2(h2, ctrl[i], key, keys[i]);
				if (found != std::numeric_limits<SizeType>::max())
				{
					contains(i, found);
					return true;
				}

				// scan slots in group for tombstone or empty
				for (SizeType j = 0; j < static_cast<SizeType>(ctrl[i].size()); ++j)
				{
					if (ctrl[i][j] == EMPTY)
					{
						// Prefer earlier tombstone if found; otherwise use this empty slot
						SizeType useGroup = (firstTombGroup != std::numeric_limits<SizeType>::max()) ? firstTombGroup : i;
						SizeType useSlot = (firstTombGroup != std::numeric_limits<SizeType>::max()) ? firstTombSlot : j;
						empty(useGroup, useSlot, h2);
						return true;
					}
					else if (ctrl[i][j] == TOMBSTONE)
					{
						if (firstTombGroup == std::numeric_limits<SizeType>::max())
						{
							firstTombGroup = i;
							firstTombSlot = j;
						}
					}
				}
				// no EMPTY in this group, continue probing
			}

			// If we found a tombstone earlier but no EMPTY was seen in any probed group, reuse it
			if (firstTombGroup != std::numeric_limits<SizeType>::max())
			{
				empty(firstTombGroup, firstTombSlot, h2);
				return true;
			}
			return false;
		}


		/// <summary>
		/// 16BYTEスロット使用のsimd版走査
		/// </summary>
		/// <typeparam name="A"></typeparam>
		/// <param name="key"></param>
		/// <param name="ctrl"></param>
		/// <param name="keys"></param>
		/// <param name="groupSize"></param>
		/// <param name="mask"></param>
		/// <param name="contains"></param>
		/// <returns></returns>
		template<class A>
		static bool InternalFind(const Key& key, const std::array<std::uint8_t, 16>* ctrl, const std::array<Key, 16>* keys, SizeType groupSize, SizeType mask, A&& contains)
		{
			auto h = HashFunc{}(key);
			SizeType idx = h & mask;
			__m128i target = _mm_set1_epi8(CalcH2FromHash(h));
			for (SizeType i = idx; i < groupSize; i++)
			{
				__m128i result = _mm_cmpeq_epi8(_mm_load_si128(reinterpret_cast<const __m128i*>(ctrl[i].data())), target);
				uint32_t resultMask = _mm_movemask_epi8(result);
				while (resultMask)
				{
					int slot = std::countr_zero(resultMask);
					if (EqualFunc{}(keys[i][slot] ,key))
					{
						contains(i, slot);
						return true;
					}
					resultMask &= resultMask - 1;
				}
			}
			for (SizeType i = 0; i < idx; i++)
			{
				__m128i result = _mm_cmpeq_epi8(_mm_load_si128(reinterpret_cast<const __m128i*>(ctrl[i].data())), target);
				uint32_t resultMask = _mm_movemask_epi8(result);
				while (resultMask)
				{
					int slot = std::countr_zero(resultMask);
					if (EqualFunc{}(keys[i][slot], key))
					{
						contains(i, slot);
						return true;
					}
					resultMask &= resultMask - 1;
				}
			}
			return false;
		}

		/// <summary>
		/// 16BYTEスロット使用のsimd版走査+挿入
		/// </summary>
		/// <typeparam name="A"></typeparam>
		/// <typeparam name="B"></typeparam>
		/// <param name="key"></param>
		/// <param name="ctrl"></param>
		/// <param name="keys"></param>
		/// <param name="groupSize"></param>
		/// <param name="mask"></param>
		/// <param name="contains"></param>
		/// <param name="empty"></param>
		/// <returns></returns>
		template<class A, class B>
		static bool InternalInsert(const Key& key, const std::array<std::uint8_t, 16>* ctrl, const std::array<Key, 16>* keys, SizeType groupSize, SizeType mask, A&& contains, B&& empty)
		{
			const auto h = HashFunc{}(key);
			const uint8_t h2 = CalcH2FromHash(h);
			const SizeType idx = h & mask;
			const __m128i target = _mm_set1_epi8(h2);
			const __m128i emptyVec = _mm_set1_epi8(EMPTY);
			const __m128i tombstoneVec = _mm_set1_epi8(TOMBSTONE);
			// Probe groups incrementally from home slot, stop when encountering an EMPTY in a group
			SizeType firstTombGroup = std::numeric_limits<SizeType>::max();
			SizeType firstTombSlot = std::numeric_limits<SizeType>::max();
			for (SizeType probe = 0; probe < groupSize; ++probe)
			{
				SizeType i = (idx + probe) & mask;
				const __m128i group = _mm_load_si128(reinterpret_cast<const __m128i*>(ctrl[i].data()));
				{
					__m128i result = _mm_cmpeq_epi8(group, target);
					uint32_t resultMask = _mm_movemask_epi8(result);
					while (resultMask)
					{
						int slot = std::countr_zero(resultMask);
						if (EqualFunc{}(keys[i][slot], key))
						{
							contains(i, slot);
							return true;
						}
						resultMask &= resultMask - 1;
					}
				}

				
				auto emptyResult = _mm_cmpeq_epi8(group, emptyVec);
				uint32_t emptyMask = _mm_movemask_epi8(emptyResult);
				if (emptyMask)
				{
					SizeType useGroup = (firstTombGroup != std::numeric_limits<SizeType>::max()) ? firstTombGroup : i;
					SizeType useSlot = (firstTombGroup != std::numeric_limits<SizeType>::max()) ? firstTombSlot : std::countr_zero(emptyMask);
					empty(useGroup, useSlot, h2);
					return true;
				}

				auto tombstoneResult = _mm_cmpeq_epi8(group, tombstoneVec);
				uint32_t tombstoneMask = _mm_movemask_epi8(emptyResult);
				if (tombstoneMask && firstTombGroup == std::numeric_limits<SizeType>::max())
				{
					firstTombGroup = i;
					firstTombSlot = std::countr_zero(tombstoneMask);
				}
			}

			// If we found a tombstone earlier but no EMPTY was seen in any probed group, reuse it
			if (firstTombGroup != std::numeric_limits<SizeType>::max())
			{
				empty(firstTombGroup, firstTombSlot, h2);
				return true;
			}
			return false;
		}
	};
}
