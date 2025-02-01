#pragma once
#include <memory>

namespace Eugene
{
	/// <summary>
	/// 可変配列(Pushやreserve無しのvectorみたいなの)
	/// </summary>
	/// <typeparam name="T"> 要素の型 </typeparam>
	template<class T>
	class Array
	{
	public:
		/// <summary>
		/// サイズ指定コンストラクタ
		/// </summary>
		/// <param name="size"> サイズ </param>
		Array(std::size_t size) :
			pointer_{ new T[size]() }, size_{ size }
		{
		}

		/// <summary>
		/// ムーブコンストラクタ
		/// </summary>
		/// <param name="array"></param>
		Array(Array&& array) noexcept :
			pointer_{ std::move(array.pointer_) }, size_{ array.size_ }
		{
			array.size_ = 0;
		}

		/// <summary>
		/// コピーコンストラクタ
		/// </summary>
		/// <param name="array"></param>
		Array(const Array& array) noexcept :
			Array{}
		{
			Resize(array.size());
			std::copy_n(array.cbegin(), array.size(), begin());
		}

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		Array() noexcept :
			size_{ 0 }
		{
		}

		/// <summary>
		/// ムーブ演算子
		/// </summary>
		/// <param name="array"></param>
		/// <returns></returns>
		Array& operator=(Array&& array) noexcept
		{
			pointer_ = std::move(array.pointer_);
			size_ = array.size_;
			array.size_ = 0;
			return *this;
		}

		/// <summary>
		/// コピー演算子
		/// </summary>
		/// <param name="array"></param>
		/// <returns></returns>
		Array& operator=(const Array& array) noexcept
		{
			Resize(array.size());
			std::copy_n(array.cbegin(), array.size(), begin());
			return *this;
		}

		/// <summary>
		/// サイズの変更
		/// </summary>
		/// <param name="size"> サイズ </param>
		constexpr void Resize(std::size_t size)
		{
			std::unique_ptr<T[]> newPointer{ new T[size]() };
			std::size_t copySize = size_;
			if (size < size_)
			{
				copySize = size;
			}

			for (std::size_t i = 0; i < copySize; i++)
			{
				new(newPointer.get() + i)T{ std::move(*(pointer_.get() + i)) };
			}
			size_ = size;
			pointer_.swap(newPointer);
		}

		/// <summary>
		/// サイズを取得
		/// </summary>
		/// <returns></returns>
		constexpr const std::size_t size() const noexcept
		{
			return size_;
		}

		/// <summary>
		/// データのポインタを取得
		/// </summary>
		/// <returns></returns>
		constexpr const T* data() const noexcept
		{
			return pointer_.get();
		}

		/// <summary>
		/// イテレーター
		/// </summary>
		struct iterator
		{
			iterator();

			iterator(Array* arrayPtr, std::size_t index) :
				arrayPtr_{ arrayPtr }, index_{ index }
			{
			}

			T& operator*()
			{
				return *(arrayPtr_->pointer_.get() + index_);
			}


			iterator& operator++()
			{
				index_++;
				return *this;
			}

			iterator& operator--()
			{
				index_--;
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
			Array* arrayPtr_;
			std::size_t index_;
		};

		/// <summary>
		/// 先頭イテレーターを取得
		/// </summary>
		/// <returns></returns>
		constexpr iterator begin()
		{
			return { this, 0 };
		}

		/// <summary>
		/// 終端のイテレーターを取得
		/// </summary>
		/// <returns></returns>
		constexpr iterator end()
		{
			return { this, size_ };
		}

		/// <summary>
		/// const版イテレーター
		/// </summary>
		struct const_iterator
		{
			const_iterator();

			const_iterator(const Array* arrayPtr, std::size_t index) :
				arrayPtr_{ arrayPtr }, index_{ index }
			{
			}

			const T& operator*() const
			{
				return *(arrayPtr_->pointer_.get() + index_);
			}


			const_iterator& operator++()
			{
				index_++;
				return *this;
			}

			const_iterator& operator--()
			{
				index_--;
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
			const Array* arrayPtr_;
			std::size_t index_;
		};

		/// <summary>
		/// const版先頭イテレーターを取得
		/// </summary>
		/// <returns></returns>
		constexpr const_iterator cbegin() const
		{
			return { this, 0 };
		}

		/// <summary>
		/// const版終端イテレーターを取得
		/// </summary>
		/// <returns></returns>
		constexpr const_iterator cend() const
		{
			return { this, size_ };
		}

		/// <summary>
		/// 配列アクセスconst参照版
		/// </summary>
		/// <param name="index"></param>
		/// <returns></returns>
		constexpr const T& operator[](std::size_t index) const&
		{
			if (index >= size_)
			{
				throw std::out_of_range{ "範囲外です" };
			}
			return *(pointer_.get() + index);
		}

		/// <summary>
		/// 配列アクセス参照版
		/// </summary>
		/// <param name="index"></param>
		/// <returns></returns>
		T& operator[](std::size_t index)&
		{
			if (index >= size_)
			{
				throw std::out_of_range{ "範囲外です" };
			}
			return *(pointer_.get() + index);
		}
	private:
		/// <summary>
		/// 配列のポインター
		/// </summary>
		std::unique_ptr<T[]> pointer_;

		/// <summary>
		/// 配列のサイズ
		/// </summary>
		std::size_t size_;

	};
}