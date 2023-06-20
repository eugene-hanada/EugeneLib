#pragma once
#include <cstdint>
#include <cassert>

namespace Eugene
{
	template<class T>
	class RefPtr;

	template<class T>
	class WeakRefPtr
	{
	public:
		WeakRefPtr(const RefPtr<T>& refptr)
		{
			ptr_ = refptr.ptr_;
			count_ = refptr.count_;
		}

		~WeakRefPtr()
		{
		}

		T* operator->() const
		{
			if (ptr_ != nullptr && count_ != nullptr)
			{
				return ptr_;
			}
			return nullptr;
		}

	private:
		T* ptr_;
		std::uint32_t* count_;
		friend WeakRefPtr;
	};

	template<class T>
	class RefPtr
	{
	public:
		RefPtr()
		{

		}

		RefPtr(T* ptr) :
			ptr_{ptr}
		{
			count_ = new std::uint32_t;
			*count_ = 1u;
		}

		RefPtr(const RefPtr& refptr) noexcept
		{
			if (*this)
			{
				*count_ -= 1u;
				if (*count_ <= 0u)
				{
					delete count_;
					delete ptr_;
					count_ = nullptr;
					ptr_ = nullptr;
				}
			}
			ptr_ = refptr.ptr_;
			count_ = refptr.count_;
			*count_ += 1u;
		}

		~RefPtr() noexcept
		{
			if (*this)
			{
				*count_ -= 1u;
				if (*count_ <= 0u)
				{
					delete count_;
					delete ptr_;
					count_ = nullptr;
					ptr_ = nullptr;
				}
			}
		}

		RefPtr& operator=(const RefPtr& refptr) noexcept
		{
			if (*this)
			{
				*count_ -= 1u;
				if (*count_ <= 0u)
				{
					delete count_;
					delete ptr_;
					count_ = nullptr;
					ptr_ = nullptr;
				}
			}
			ptr_ = refptr.ptr_;
			count_ = refptr.count_;
			*count_ += 1u;
			return *this;
		}

		RefPtr& operator=(T* ptr) noexcept
		{
			if (*this)
			{
				*count_ -= 1u;
				if (*count_ <= 0u)
				{
					delete count_;
					delete ptr_;
					count_ = nullptr;
					ptr_ = nullptr;
				}
			}
			ptr_ = ptr;
			count_ = new std::uint32_t;
			*count_ = 1u;
			return *this;
		}

		explicit operator bool() const noexcept 
		{
			return count_ != nullptr && ptr_ != nullptr; 
		}

		bool operator!() const noexcept 
		{
			return !static_cast<bool>(*this);
		}

		const T& Cref(void) const
		{
			if (!static_cast<bool>(*this))
			{
				assert(false);
			}
			return *ptr_;
		}

		T& operator*(void)
		{
			if (!static_cast<bool>(*this))
			{
				assert(false);
			}
			return *ptr_;
		}

		T* operator->(void) const noexcept
		{
			return *ptr_;
		}


	private:
		T* ptr_{nullptr};
		std::uint32_t* count_{nullptr};


		friend class RefPtr<T>;

		friend class WeakRefPtr<T>;
	};

	
}
