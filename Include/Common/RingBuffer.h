#pragma once
#include <vector>
#include "../Math/Geometry.h"

namespace Eugene
{
	template<class T>
	class RingBuffer
	{
	public:
		class iterator
		{
		public:
			iterator()
			{
			}

			iterator(std::span<T> span, std::uint64_t index, std::uint64_t startIndex) :
				span_{span}, index_{index}, startIndex_{startIndex}
			{
			}

			T& operator*()
			{
				return span_[(startIndex_ + index_) & (span_.size() - 1ull)];
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
			std::span<T> span_;
			std::uint64_t index_;
			std::uint64_t startIndex_;
		};


		RingBuffer(std::uint64_t size) :
			buffer_(0), writeIndex_{ 0ull }, readIndex_{ 0ull }
		{
			buffer_.resize(AlignmentedSize(size, 2ull));
		}

		RingBuffer(std::initializer_list<T> initList) :
			buffer_(initList), writeIndex_{ initList.size()}, readIndex_{0ull}
		{

		}


		void Push(const T& value)
		{
			if (writeIndex_ - readIndex_ >= buffer_.size()) 
			{
				readIndex_++;
			}
			buffer_[writeIndex_++ & (buffer_.size() - 1ull)] = value;
		}
		
		void Emplace(T&&value)
		{
			if (writeIndex_ - readIndex_ >= buffer_.size())
			{
				readIndex_++;
			}
			buffer_[writeIndex_++ & (buffer_.size() - 1ull)] = std::move(value);
		}

		void Pop()
		{
			readIndex_++;
		}

		T& Front()
		{
			return  buffer_[readIndex_ & (buffer_.size() - 1ull)] ;
		}

		const T& Front() const
		{
			return buffer_.at(readIndex_ & (buffer_.size() - 1ull));
		}

		T& Back()
		{
			return buffer_[(writeIndex_ - 1u) & (buffer_.size() - 1ull)];
		}

		const T& Back() const
		{
			return buffer_.at((writeIndex_ - 1u) & (buffer_.size() - 1ull));
		}

		uint64 Size() const
		{
			return std::min(writeIndex_ - readIndex_, buffer_.size());
		}


		iterator begin()
		{
			return { buffer_,0ull , readIndex_};
		}

		iterator end()
		{
			return { buffer_,buffer_.size(),readIndex_};
		}
	private:

		std::vector<T> buffer_;
		std::uint64_t writeIndex_;
		std::uint64_t readIndex_;
	};
}

