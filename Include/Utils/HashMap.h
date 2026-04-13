#pragma once
#include <cstdint>
#include<vector>
#include "Array.h"

namespace Eugene
{
	/// <summary>
	/// 固定サイズの軽量なハッシュマップ（キー衝突を想定していないので注意）
	/// </summary>
	/// <typeparam name="IndexSizeType"> インデックスの型 </typeparam>
	/// <typeparam name="Value"> 値の型 </typeparam>
	/// <typeparam name="HashFunc"> ハッシュ関数の型 </typeparam>
	/// <typeparam name="Key"> キーの型 </typeparam>
	template<class Value, class Key, class HashFunc, class IndexSizeType = uint32_t>
	class FixedHashMap
	{
	public:
		struct iterator
		{
			iterator() :
				map_{ nullptr }, index_{ 0 }
			{
			}
			iterator(const FixedHashMap* map, const IndexSizeType index) :
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
			const FixedHashMap* map_;
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

		iterator begin() const
		{
			return { this, 0 };
		}

		iterator end() const
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

		// 外部から設定
		Array<Value, IndexSizeType> dataTable_;   // データテーブル
		IndexSizeType mask = 0;			// table_size - 1
	};
}
