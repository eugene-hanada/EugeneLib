#pragma once
#include <mimalloc.h>
#include <memory>
#include <span>
#include <iterator>
#include "../Debug/Debug.h"

namespace Eugene
{
	/// <summary>
	/// 可変配列(Pushやreserve無しのvectorみたいなの)
	/// </summary>
	/// <typeparam name="T"> 要素の型 </typeparam>
	template<class T, class SizeType = std::uint32_t>
	class Array
	{
	public:
		/// <summary>
		/// サイズ指定コンストラクタ
		/// </summary>
		/// <param name="size"> サイズ </param>
		Array(SizeType size) :
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
		constexpr T* data() const noexcept
		{
			return pointer_.get();
		}

		/// <summary>
		/// イテレーター
		/// </summary>
		struct iterator
		{
			using iterator_category = std::random_access_iterator_tag;
			using value_type = T;
			using difference_type = std::ptrdiff_t;
			using pointer = T*;
			using reference = T&;

			iterator() : arrayPtr_{ nullptr }, index_{ 0 } {}

			iterator(Array* arrayPtr, std::size_t index) :
				arrayPtr_{ arrayPtr }, index_{ index }
			{
			}

			reference operator*() const
			{
				return *(arrayPtr_->pointer_.get() + index_);
			}

			iterator& operator++()
			{
				++index_;
				return *this;
			}

			iterator operator++(int)
			{
				iterator tmp = *this;
				++*this;
				return tmp;
			}

			iterator& operator--()
			{
				--index_;
				return *this;
			}

			iterator operator--(int)
			{
				iterator tmp = *this;
				--*this;
				return tmp;
			}

			iterator& operator+=(difference_type n)
			{
				if (n >= 0)
					index_ += static_cast<std::size_t>(n);
				else
					index_ -= static_cast<std::size_t>(-n);
				return *this;
			}

			iterator& operator-=(difference_type n)
			{
				return *this += -n;
			}

			iterator operator+(difference_type n) const
			{
				iterator tmp = *this;
				tmp += n;
				return tmp;
			}

			iterator operator-(difference_type n) const
			{
				iterator tmp = *this;
				tmp -= n;
				return tmp;
			}

			difference_type operator-(const iterator& rhs) const
			{
				return static_cast<difference_type>(index_) - static_cast<difference_type>(rhs.index_);
			}

			reference operator[](difference_type n) const
			{
				return *(*this + n);
			}

			bool operator==(const iterator& itr) const
			{
				return index_ == itr.index_;
			}

			bool operator!=(const iterator& itr) const
			{
				return index_ != itr.index_;
			}

			bool operator<(const iterator& itr) const { return index_ < itr.index_; }
			bool operator<=(const iterator& itr) const { return index_ <= itr.index_; }
			bool operator>(const iterator& itr) const { return index_ > itr.index_; }
			bool operator>=(const iterator& itr) const { return index_ >= itr.index_; }

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
			using iterator_category = std::random_access_iterator_tag;
			using value_type = T;
			using difference_type = std::ptrdiff_t;
			using pointer = const T*;
			using reference = const T&;

			const_iterator() : arrayPtr_{ nullptr }, index_{ 0 } {}

			const_iterator(const Array* arrayPtr, std::size_t index) :
				arrayPtr_{ arrayPtr }, index_{ index }
			{
			}

			reference operator*() const
			{
				return *(arrayPtr_->pointer_.get() + index_);
			}

			const_iterator& operator++()
			{
				++index_;
				return *this;
			}

			const_iterator operator++(int)
			{
				const_iterator tmp = *this;
				++*this;
				return tmp;
			}

			const_iterator& operator--()
			{
				--index_;
				return *this;
			}

			const_iterator operator--(int)
			{
				const_iterator tmp = *this;
				--*this;
				return tmp;
			}

			const_iterator& operator+=(difference_type n)
			{
				if (n >= 0)
					index_ += static_cast<std::size_t>(n);
				else
					index_ -= static_cast<std::size_t>(-n);
				return *this;
			}

			const_iterator& operator-=(difference_type n)
			{
				return *this += -n;
			}

			const_iterator operator+(difference_type n) const
			{
				const_iterator tmp = *this;
				tmp += n;
				return tmp;
			}

			const_iterator operator-(difference_type n) const
			{
				const_iterator tmp = *this;
				tmp -= n;
				return tmp;
			}

			difference_type operator-(const const_iterator& rhs) const
			{
				return static_cast<difference_type>(index_) - static_cast<difference_type>(rhs.index_);
			}

			reference operator[](difference_type n) const
			{
				return *(*this + n);
			}

			bool operator==(const const_iterator& itr) const
			{
				return index_ == itr.index_;
			}

			bool operator!=(const const_iterator& itr) const
			{
				return index_ != itr.index_;
			}

			bool operator<(const const_iterator& itr) const { return index_ < itr.index_; }
			bool operator<=(const const_iterator& itr) const { return index_ <= itr.index_; }
			bool operator>(const const_iterator& itr) const { return index_ > itr.index_; }
			bool operator>=(const const_iterator& itr) const { return index_ >= itr.index_; }

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
			EUGENE_ASSERT_MSG(index < size_, "範囲外です");
			return *(pointer_.get() + index);
		}

		/// <summary>
		/// 配列アクセス参照版
		/// </summary>
		/// <param name="index"></param>
		/// <returns></returns>
		T& operator[](std::size_t index)&
		{
			EUGENE_ASSERT_MSG(index < size_, "範囲外です");
			return *(pointer_.get() + index);
		}

		constexpr const std::span<T> AsSpan() const & noexcept
		{
			return { pointer_.get(), size_ };
		}
	private:
		/// <summary>
		/// 配列のポインター
		/// </summary>
		std::unique_ptr<T[]> pointer_;

		/// <summary>
		/// 配列のサイズ
		/// </summary>
		SizeType size_;

	};

	/// <summary>
	/// vectorっぽいやつ（サイズがsize_t型以外にしたいときにでも）
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <typeparam name="SizeType"></typeparam>
	/// <typeparam name="PushReserve"></typeparam>
	template<class T, class SizeType = std::uint32_t, SizeType PushReserve = 2>
	class Vector
	{
	public:
		struct iterator
		{
			iterator() :
				vectorPtr_{ nullptr }, index_{ 0 }
			{
			}
			iterator(Vector* vectorPtr, SizeType index) :
				vectorPtr_{ vectorPtr }, index_{ index }
			{
			}
			T& operator*()
			{
				return *(vectorPtr_->pointer_.get() + index_);
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
			Vector* vectorPtr_;
			SizeType index_;
			friend class Vector;
		};

		struct const_iterator
		{
			using iterator_category = std::random_access_iterator_tag;
			using value_type = T;
			using difference_type = std::ptrdiff_t;
			using pointer = const T*;
			using reference = const T&;

			const_iterator() : vectorPtr_{ nullptr }, index_{ 0 } {}

			const_iterator(const Vector* vectorPtr, SizeType index) :
				vectorPtr_{ vectorPtr }, index_{ index }
			{
			}

			reference operator*() const
			{
				return *(vectorPtr_->pointer_.get() + index_);
			}

			const_iterator& operator++()
			{
				++index_;
				return *this;
			}

			const_iterator operator++(int)
			{
				const_iterator tmp = *this;
				++*this;
				return tmp;
			}

			const_iterator& operator--()
			{
				--index_;
				return *this;
			}

			const_iterator operator--(int)
			{
				const_iterator tmp = *this;
				--*this;
				return tmp;
			}

			const_iterator& operator+=(difference_type n)
			{
				if (n >= 0)
					index_ += static_cast<SizeType>(n);
				else
					index_ -= static_cast<SizeType>(-n);
				return *this;
			}

			const_iterator& operator-=(difference_type n)
			{
				return *this += -n;
			}

			const_iterator operator+(difference_type n) const
			{
				const_iterator tmp = *this;
				tmp += n;
				return tmp;
			}

			const_iterator operator-(difference_type n) const
			{
				const_iterator tmp = *this;
				tmp -= n;
				return tmp;
			}

			difference_type operator-(const const_iterator& rhs) const
			{
				return static_cast<difference_type>(index_) - static_cast<difference_type>(rhs.index_);
			}

			reference operator[](difference_type n) const
			{
				return *(*this + n);
			}

			bool operator==(const const_iterator& itr) const
			{
				return index_ == itr.index_;
			}

			bool operator!=(const const_iterator& itr) const
			{
				return index_ != itr.index_;
			}

			bool operator<(const const_iterator& itr) const { return index_ < itr.index_; }
			bool operator<=(const const_iterator& itr) const { return index_ <= itr.index_; }
			bool operator>(const const_iterator& itr) const { return index_ > itr.index_; }
			bool operator>=(const const_iterator& itr) const { return index_ >= itr.index_; }

		private:
			const Vector* vectorPtr_;
			SizeType index_;
			friend class Vector;
		};

		Vector() : size_{ 0 }, capacity_{ 0 }
		{
		}

		Vector(const SizeType size) : size_{ size }, capacity_{ size }, pointer_{ new T[size]() }
		{
		}

		Vector(std::initializer_list<T> init) : size_{ static_cast<SizeType>(init.size()) }, capacity_{ size_ }, pointer_{ new T[size_] }
		{
			std::copy(init.begin(), init.end(), pointer_.get());
		}

		Vector(const Vector& vector) : size_{ vector.size_ }, capacity_{ vector.capacity_ }, pointer_{ new T[vector.capacity_]() }
		{
			std::copy_n(vector.pointer_.get(), size_, pointer_.get());
		}

		Vector(Vector&& vector) noexcept : size_{ vector.size_ }, capacity_{ vector.capacity_ }, pointer_{ std::move(vector.pointer_) }
		{
			vector.size_ = 0;
			vector.capacity_ = 0;
		}

		Vector& operator=(const Vector& vector)
		{
			if (this != &vector)
			{
				size_ = vector.size_;
				capacity_ = vector.capacity_;
				pointer_.reset(new T[capacity_]());
				std::copy_n(vector.pointer_.get(), size_, pointer_.get());
			}
			return *this;
		}

		Vector& operator=(Vector&& vector) noexcept
		{
			if (this != &vector)
			{
				size_ = vector.size_;
				capacity_ = vector.capacity_;
				pointer_ = std::move(vector.pointer_);
				vector.size_ = 0;
				vector.capacity_ = 0;
			}
			return *this;
		}

		T& operator[](const SizeType index) &
		{
			EUGENE_ASSERT_MSG(index < size_, "範囲外です");
			return *(pointer_.get() + index);
		}

		const T& operator[](const SizeType index) const&
		{
			EUGENE_ASSERT_MSG(index < size_, "範囲外です");
			return *(pointer_.get() + index);
		}

		const size_t size() const
		{
			return size_;
		}

		iterator begin()
		{
			return { this, 0 };
		}

		iterator end()
		{
			return { this, size_ };
		}

		const_iterator cbegin() const
		{
			return { this, 0 };
		}

		const_iterator cend() const
		{
			return { this, size_ };
		}

		void Reserve(const SizeType capacity)
		{
			if (capacity > capacity_)
			{
				std::unique_ptr<T[]> newPointer{ new T[capacity]() };
				for (SizeType i = 0; i < size_; i++)
				{
					new(newPointer.get() + i)T{ std::move(*(pointer_.get() + i)) };
				}
				capacity_ = capacity;
				pointer_.swap(newPointer);
			}
		}

		void Resize(const SizeType size)
		{
			if (size <= size_)
			{
				for (SizeType i = size; i < size_; i++)
				{
					(pointer_.get() + i)->~T();
				}
			}
			else
			{
				if (size > capacity_)
				{
					Reserve(size);

				}
				for (SizeType i = size_; i < size; i++)
				{
					new(pointer_.get() + i)T{};
				}
			}
			size_ = size;
		}

		void PushBack(const T& value)
		{
			if (size_ == capacity_)
			{
				Reserve(capacity_ == 0 ? 1 : capacity_ * PushReserve);
			}
			new(pointer_.get() + size_)T{ value };
			size_++;
		}

		void EmplaceBack(T&& value)
		{
			if (size_ == capacity_)
			{
				Reserve(capacity_ == 0 ? 1 : capacity_ * PushReserve);
			}
			new(pointer_.get() + size_)T{ std::move(value) };
			size_++;
		}

		void Insert(SizeType index, const T& value)
		{
			EUGENE_ASSERT_MSG(index <= size_, "範囲外です");
			if (size_ == capacity_)
			{
				Reserve(capacity_ == 0 ? 1 : capacity_ * 2);
			}
			for (SizeType i = size_; i > index; i--)
			{
				new(pointer_.get() + i)T{ std::move(*(pointer_.get() + i - 1)) };
				(pointer_.get() + i - 1)->~T();
			}
			new(pointer_.get() + index)T{ value };
			size_++;
		}

		void Insert(iterator itr, const T& value)
		{
			Insert(itr.index_, value);
		}

		void Emplace(SizeType index, T&& value)
		{
			EUGENE_ASSERT_MSG(index <= size_, "範囲外です");
			if (size_ == capacity_)
			{
				Reserve(capacity_ == 0 ? 1 : capacity_ * 2);
			}
			for (SizeType i = size_; i > index; i--)
			{
				new(pointer_.get() + i)T{ std::move(*(pointer_.get() + i - 1)) };
				(pointer_.get() + i - 1)->~T();
			}
			new(pointer_.get() + index)T{ std::move(value) };
			size_++;
		}

		void Emplace(iterator itr, T&& value)
		{
			Emplace(itr.index_, std::move(value));
		}

		void Erase(SizeType index)
		{
			EUGENE_ASSERT_MSG(index < size_, "範囲外です");
			(pointer_.get() + index)->~T();
			for (SizeType i = index; i < size_ - 1; i++)
			{
				new(pointer_.get() + i)T{ std::move(*(pointer_.get() + i + 1)) };
				(pointer_.get() + i + 1)->~T();
			}
			size_--;
		}

		void Erase(iterator itr)
		{
			Erase(itr.index_);
		}

		void PopBack()
		{
			EUGENE_ASSERT_MSG(size_ > 0, "要素がありません");
			size_--;
			(pointer_.get() + size_)->~T();
		}

		void Clear()
		{
			for (SizeType i = 0; i < size_; i++)
			{
				(pointer_.get() + i)->~T();
			}
			size_ = 0;
		}

		void Assign(SizeType count, const T& value)
		{
			Clear();
			if (count > capacity_)
			{
				Reserve(count);
			}
			for (SizeType i = 0; i < count; i++)
			{
				new(pointer_.get() + i)T{ value };
			}
			size_ = count;
		}

		void Assign(iterator first, iterator last)
		{
			Clear();
			SizeType count = 0;
			for (auto it = first; it != last; ++it)
			{
				PushBack(*it);
				count++;
			}
		}

	private:
		SizeType size_;
		SizeType capacity_;
		std::unique_ptr<T[]> pointer_;
	};
}
