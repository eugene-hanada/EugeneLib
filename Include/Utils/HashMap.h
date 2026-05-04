#pragma once
#include <concepts>
#include <cstdint>
#include <mimalloc.h>
#include<vector>
#include "Array.h"
#include "Hash.h"


#include <functional>
#include <cassert>

namespace Eugene
{
	template<class Key, std::unsigned_integral HashValueType>
	class HashCalcFunc
	{
	public:
		using ReturnValueType = HashValueType;
		ReturnValueType operator()(const Key& key) const
		{
			if constexpr (std::is_same<HashValueType, std::uint64_t>::value)
			{
				return FNV1aHash64(key);
			}
			else
			{
				return FNV1aHash32(key);
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
	template<class Key, class Value, std::unsigned_integral SizeType = std::uint32_t, class HashFunc = HashCalcFunc<Key, std::uint64_t>, class EqualFunc = std::equal_to<Key>>
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
			std::pair<Key&, Value&> operator*() noexcept
			{
				return { map_->keys_[groupIndex_][slotIndex_], map_->values_[groupIndex_][slotIndex_] };
			}
			Key& first() noexcept
			{
				return map_->keys_[groupIndex_][slotIndex_];
			}
			Value& second() noexcept
			{
				return map_->values_[groupIndex_][slotIndex_];
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
			const Key& first() const
			{
				return map_->keys_[groupIndex_][slotIndex_];
			}
			const Value& second() const
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
			auto h = HashFunc{}(key);
			SizeType idx = h & mask;
			uint8_t h2 = CalcH2FromHash(h);
			
			for(SizeType i = idx; i < groupSize_; i++)
			{
				SizeType result = FindH2(h2, key, i);
				if (result != std::numeric_limits<SizeType>::max())
				{
					return values_[i][result];
				}
			}
			for (SizeType i = 0; i < idx; i++)
			{
				SizeType result = FindH2(h2, key, i);
				if (result != std::numeric_limits<SizeType>::max())
				{
					return values_[i][result];
				}
			}

			// 空きスロットを探す
			for (SizeType i = idx; i < groupSize_; i++)
			{
				auto searchResult = SearchEmptyCtrl(ctrl_[i]);
				if (searchResult != std::numeric_limits<SizeType>::max())
				{
					ctrl_[i][searchResult] = h2;
					new(&keys_[i][searchResult]) Key{ key };
					new(&values_[i][searchResult]) Value{};
					size_++;
					return values_[i][searchResult];
				}
			}
			for (SizeType i = 0; i < idx; i++)
			{
				auto searchResult = SearchEmptyCtrl(ctrl_[i]);
				if (searchResult != std::numeric_limits<SizeType>::max())
				{
					ctrl_[i][searchResult] = h2;
					new(&keys_[i][searchResult]) Key{ key };
					new(&values_[i][searchResult]) Value();
					size_++;
					return values_[i][searchResult];
				}
			}
			EUGENE_ASSERT(false); // 空きスロットが見つからない場合は再ハッシュが必要
		}

		const Value& At(const Key& key) const & noexcept
		{
			auto h = HashFunc{}(key);
			SizeType idx = h & mask;
			uint8_t h2 = CalcH2FromHash(h);

			for (SizeType i = idx; i < groupSize_; i++)
			{
				SizeType result = FindH2(h2, key, i);
				if (result != std::numeric_limits<SizeType>::max())
				{
					return values_[i][result];
				}
			}
			for (SizeType i = 0; i < idx; i++)
			{
				SizeType result = FindH2(h2, key, i);
				if (result != std::numeric_limits<SizeType>::max())
				{
					return values_[i][result];
				}
			}
			EUGENE_ASSERT_MSG(false, "Key not found"); // キーが見つからない場合は例外を投げるか、エラー処理を行う
		}

		bool Contains(const Key& key) const noexcept
		{
			auto h = HashFunc{}(key);
			SizeType idx = h & mask;
			uint8_t h2 = CalcH2FromHash(h);
			for (SizeType i = idx; i < groupSize_; i++)
			{
				SizeType result = FindH2(h2, key, i);
				if (result != std::numeric_limits<SizeType>::max())
				{
					return true;
				}
			}
			for (SizeType i = 0; i < idx; i++)
			{
				SizeType result = FindH2(h2, key, i);
				if (result != std::numeric_limits<SizeType>::max())
				{
					return true;
				}
			}
			return false;
		}

		bool Insert(const Key& key, const Value& value) noexcept
		{
			auto h = HashFunc{}(key);
			SizeType idx = h & mask;
			uint8_t h2 = CalcH2FromHash(h);
			for (SizeType i = idx; i < groupSize_; i++)
			{
				SizeType result = FindH2(h2, key, i);
				if (result != std::numeric_limits<SizeType>::max())
				{
					values_[i][result] = value;
					return true;
				}
			}
			for (SizeType i = 0; i < idx; i++)
			{
				SizeType result = FindH2(h2, key, i);
				if (result != std::numeric_limits<SizeType>::max())
				{
					values_[i][result] = value;
					return true;
				}
			}
			// 格納できない場合はリハッシュ
			if (size_ >= (groupSize_ * ctrlArraySize))
			{
				Rehash(groupSize_ * 2);
				idx = h & mask;
			}
			for (SizeType i = idx; i < groupSize_; i++)
			{
				SizeType result = SearchEmptyCtrl(ctrl_[i]);
				if (result != std::numeric_limits<SizeType>::max())
				{
					ctrl_[i][result] = h2;
					new(&keys_[i][result]) Key{ key };
					new(&values_[i][result]) Value{ value };
					size_++;
					return true;
				}
			}
			for (SizeType i = 0; i < idx; i++)
			{
				SizeType result = SearchEmptyCtrl(ctrl_[i]);
				if (result != std::numeric_limits<SizeType>::max())
				{
					ctrl_[i][result] = h2;
					new(&keys_[i][result]) Key{ key };
					new(&values_[i][result]) Value{ value };
					size_++;
					return true;
				}
			}
			return false;
		}

		bool Emplace(const Key& key, Value&& value) noexcept
		{
			auto h = HashFunc{}(key);
			SizeType idx = h & mask;
			uint8_t h2 = CalcH2FromHash(h);
			for (SizeType i = idx; i < groupSize_; i++)
			{
				SizeType result = FindH2(h2, key, i);
				if (result != std::numeric_limits<SizeType>::max())
				{
					values_[i][result] = std::move(value);
					return true;
				}
			}
			for (SizeType i = 0; i < idx; i++)
			{
				SizeType result = FindH2(h2, key, i);
				if (result != std::numeric_limits<SizeType>::max())
				{
					values_[i][result] = std::move(value);
					return true;
				}
			}
			// 格納できない場合はリハッシュ
			if (size_ >= (groupSize_ * ctrlArraySize))
			{
				Rehash(groupSize_ * 2);
				idx = h & mask;
			}
			// 空きスロットを探す
			for (SizeType i = idx; i < groupSize_; i++)
			{
				SizeType result = SearchEmptyCtrl(ctrl_[i]);
				if (result != std::numeric_limits<SizeType>::max())
				{
					ctrl_[i][result] = h2;
					new(&keys_[i][result]) Key{ key };
					new(&values_[i][result]) Value{ value };
					size_++;
					return true;
				}
			}
			for (SizeType i = 0; i < idx; i++)
			{
				SizeType result = SearchEmptyCtrl(ctrl_[i]);
				if (result != std::numeric_limits<SizeType>::max())
				{
					ctrl_[i][result] = h2;
					new(&keys_[i][result]) Key{ key };
					new(&values_[i][result]) Value{ value };
					size_++;
					return true;
				}
			}
			return false;
		}

		void Erace(const Key& key) noexcept
		{
			auto h = HashFunc{}(key);
			SizeType idx = h & mask;
			uint8_t h2 = CalcH2FromHash(h);
			for (SizeType i = idx; i < groupSize_; i++)
			{
				SizeType result = FindH2(h2, key, i);
				if (result != std::numeric_limits<SizeType>::max())
				{
					ctrl_[i][result] = TOMBSTONE;
					keys_[i][result].~Key();
					values_[i][result].~Value();
					size_--;
					return;
				}
			}
			for (SizeType i = 0; i < idx; i++)
			{
				SizeType result = FindH2(h2, key, i);
				if (result != std::numeric_limits<SizeType>::max())
				{
					ctrl_[i][result] = TOMBSTONE;
					keys_[i][result].~Key();
					values_[i][result].~Value();
					size_--;
					return;
				}
			}
		}

		SizeType size() const noexcept { return size_; }

		iterator Find(const Key& key) & noexcept
		{
			auto h = HashFunc{}(key);
			SizeType idx = h & mask;
			uint8_t h2 = CalcH2FromHash(h);
			for (SizeType i = idx; i < groupSize_; i++)
			{
				SizeType result = FindH2(h2, key, i);
				if (result != std::numeric_limits<SizeType>::max())
				{
					return iterator(this, i, result);
				}
			}
			for (SizeType i = 0; i < idx; i++)
			{
				SizeType result = FindH2(h2, key, i);
				if (result != std::numeric_limits<SizeType>::max())
				{
					return iterator(this, i, result);
				}
			}
			return end();
		}

		iterator begin() noexcept
		{
			return iterator(this, 0, 0);
		}

		iterator end() noexcept
		{
			return iterator(this, groupSize_, sizeof(MetaDataType));
		}

		const_iterator cbegin() const noexcept
		{
			return const_iterator(this, 0, 0);
		}

		const_iterator cend() const noexcept
		{
			return const_iterator(this, groupSize_, sizeof(MetaDataType));
		}

	private:
		using MetaDataType = std::uint64_t;
		static constexpr SizeType ctrlArraySize = sizeof(MetaDataType);
		using CtrlArrayType = std::array<std::uint8_t, ctrlArraySize>;
		using ValueArrayType = std::array<Value, ctrlArraySize>;
		using KeyArrayType = std::array<Key, ctrlArraySize>;

		static constexpr uint8_t EMPTY = 0x80;
		static constexpr uint8_t TOMBSTONE = 0xFE;

		KeyArrayType* keys_;
		ValueArrayType* values_;
		CtrlArrayType* ctrl_;

		SizeType mask = 0;
		SizeType groupSize_;
		SizeType size_ = 0;
	public:
		template<std::unsigned_integral T>
		static constexpr uint8_t CalcH2FromHash(const T h) noexcept
		{
			return static_cast<uint8_t>(h & 0x7F); // 下位7bit
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

		static SizeType MatchH2(const CtrlArrayType& ctrl, uint8_t h2) noexcept
		{
			SizeType x;
			std::memcpy(&x, ctrl.data(), sizeof(x)); // 安全にロード

			// key を全バイトに展開
			SizeType k = 0x0101010101010101ULL * h2;

			SizeType y = x ^ k; // 一致バイトは0になる

			// 各バイトが0かどうかを検出
			SizeType tmp = (y - static_cast<SizeType>(0x0101010101010101ULL)) & ~y & static_cast<SizeType>(0x8080808080808080ULL);

			return tmp; // 各バイトのMSBにフラグが立つ
		}

		static std::pair<SizeType, SizeType> SearchEmpty(const CtrlArrayType* ctrl, const KeyArrayType* keys, SizeType groupSize) noexcept
		{
			for (SizeType i = 0; i < groupSize; i++)
			{
				for (SizeType j = 0; j < static_cast<SizeType>(ctrl[i].size()); j++)
				{
					if (ctrl[i][j] != EMPTY || ctrl[i][j] != TOMBSTONE)
					{
						return { i, j };
					}
				}
			}
			return { std::numeric_limits<SizeType>::max(), std::numeric_limits<SizeType>::max() };
		}

		SizeType FindH2(const std::uint8_t h2, const Key& key, SizeType groupIndex) const noexcept
		{
			auto result = MatchH2(ctrl_[groupIndex], h2);
			for (SizeType slot = std::countr_zero(result) >> 3; result != 0; result &= (result - 1))
				{
					if (EqualFunc{}(keys_[groupIndex][slot], key))
					{
						return slot;
					}
				}
			return std::numeric_limits<SizeType>::max(); // キーが見つからなかった
		}

		void Rehash(SizeType newGroupSize) noexcept
		{
#ifndef EUGENE_TEST
			auto newKeys = static_cast<KeyArrayType*>(mi_new_nothrow(sizeof(KeyArrayType) * newGroupSize));
			auto newValues = static_cast<ValueArrayType*>(mi_new_nothrow(sizeof(ValueArrayType) * newGroupSize));
			auto newCtrl = static_cast<CtrlArrayType*>(mi_new_nothrow(sizeof(CtrlArrayType) * newGroupSize));
#else
			auto newKeys = static_cast<KeyArrayType*>(std::malloc(sizeof(KeyArrayType) * newGroupSize));
			auto newValues = static_cast<ValueArrayType*>(std::malloc(sizeof(ValueArrayType) * newGroupSize));
			auto newCtrl = static_cast<CtrlArrayType*>(std::malloc(sizeof(CtrlArrayType) * newGroupSize));
#endif
			EUGENE_ASSERT(newKeys != nullptr && newValues != nullptr && newCtrl != nullptr); // メモリ確保に失敗した場合はアサートかける
			for (SizeType i = 0; i < newGroupSize; i++)
			{
				newCtrl[i].fill(EMPTY);
			}
			auto newMask = newGroupSize - 1;
			
			if (keys_ != nullptr)
			{
				// 新しいグループサイズに基づいて再ハッシュ
				for (SizeType i = 0; i < groupSize_; i++)
				{
					for (SizeType j = 0; j < static_cast<SizeType>(ctrl_[i].size()); j++)
					{
						if (ctrl_[i][j] == EMPTY || ctrl_[i][j] == TOMBSTONE) continue;

						auto hash = HashFunc{}(keys_[i][j]);
						auto h2 = CalcH2FromHash(hash);
						
						
						SizeType newIdx = hash & (newGroupSize - 1);
						bool inserted = false;
						//SizeType ctrlIndex = std::numeric_limits<SizeType>::max();
						
						for (SizeType k = newIdx; k < newGroupSize; k++)
						{
							auto searchResult = SearchEmptyCtrl(newCtrl[k]);
							if (searchResult != std::numeric_limits<SizeType>::max())
							{
								new(&newKeys[newIdx][searchResult]) Key(std::move(keys_[i][j]));
								keys_[i][j].~Key();
								new(&newValues[newIdx][searchResult]) Value(std::move(values_[i][j]));
								values_[i][j].~Value();
								newCtrl[newIdx][searchResult] = h2;
								inserted = true;
								break;
							}
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

			keys_ = newKeys;
			values_ = newValues;
			ctrl_ = newCtrl;
			mask = newGroupSize - 1;
			groupSize_ = newGroupSize;
		}
	};
}
