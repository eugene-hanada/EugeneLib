#pragma once
#include <vector>
#include <array>
#include <atomic>
#include "Array.h"


namespace Eugene
{
	/// <summary>
	/// リングバッファ
	/// </summary>
	/// <typeparam name="T"> 要素の型 </typeparam>
	template<class T, class SizeType = std::uint32_t>
	class RingBuffer
	{
	public:

		/// <summary>
		/// イテレーター
		/// </summary>
		class iterator
		{
		public:
			iterator():
				index_{0ull},startIndex_{0ull}
			{
			}

			iterator(std::span<T> span, const SizeType index, const SizeType startIndex) :
				span_{span}, index_{index}, startIndex_{startIndex}
			{
			}

			T& operator*()
			{
				return span_[(startIndex_ + index_) & (span_.size() - 1)];
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

			/// <summary>
			/// バッファの参照
			/// </summary>
			std::span<T> span_;

			
			/// <summary>
			/// インデックスX
			/// </summary>
			SizeType index_;

			/// <summary>
			/// 開始インデックス
			/// </summary>
			const SizeType startIndex_;
		};

		class const_iterator
		{
		public :
			const_iterator() :
				index_{0}, startIndex_{0}
			{
			}

			const_iterator(std::span<T> span, const SizeType index, const SizeType startIndex) :
				span_{ span }, index_{ index }, startIndex_{ startIndex }
			{
			}

			const T& operator*() const
			{
				return span_[(startIndex_ + index_) & (span_.size() - 1)];
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

			bool operator==(const const_iterator& itr) const
			{
				return index_ == itr.index_;
			}

			bool operator!=(const const_iterator& itr) const
			{
				return index_ != itr.index_;
			}
		private:
			/// <summary>
			/// バッファの参照
			/// </summary>
			std::span<const T> span_;


			/// <summary>
			/// インデックスX
			/// </summary>
			SizeType index_;

			/// <summary>
			/// 開始インデックス
			/// </summary>
			const SizeType startIndex_;
		};

		/// <summary>
		/// サイズ指定コンストラクタ
		/// </summary>
		/// <param name="size"> サイズ </param>
		RingBuffer(const SizeType size) :
			buffer_(0), writeIndex_{ 0ull }, readIndex_{ 0ull }
		{
			buffer_.Resize(1ull << static_cast<SizeType>(std::ceil(std::log2(size))));
		}

		/// <summary>
		/// 初期化リスト使用のコンストラクタ
		/// </summary>
		/// <param name="initList"> 初期化リスト </param>
		RingBuffer(std::initializer_list<T> initList) :
			buffer_(initList), writeIndex_{ static_cast<SizeType>(initList.size())}, readIndex_{0}
		{

		}


		/// <summary>
		/// サイズを変更する
		/// </summary>
		/// <param name="size"> 変更後のサイズ </param>
		void Resize(const SizeType size)
		{
			size = 1 << static_cast<SizeType>(std::ceil(std::log2(size)));
			auto nowSize = Size();
			for (SizeType i = 0; i < nowSize - 1; i++)
			{
				std::swap(buffer_[i], buffer_[(readIndex_ + i) & (buffer_.size() - 1)]);
			}
			
			readIndex_ = 0;
			writeIndex_ = std::min(nowSize, size);
			buffer_.Resize(size);
		}

		/// <summary>
		/// 要素を入れる
		/// </summary>
		/// <param name="value"> 要素 </param>
		void Push(const T& value)
		{
			if (writeIndex_ - readIndex_ >= buffer_.size()) 
			{
				readIndex_++;
			}
			buffer_[writeIndex_++ & (buffer_.size() - 1ull)] = value;
		}
		
		/// <summary>
		/// 要素を入れる
		/// </summary>
		/// <param name="value"></param>
		void Emplace(T&&value)
		{
			if (writeIndex_ - readIndex_ >= buffer_.size())
			{
				readIndex_++;
			}
			buffer_[writeIndex_++ & (buffer_.size() - 1)] = std::move(value);
		}

		/// <summary>
		/// 要素を削除する
		/// </summary>
		void Pop()
		{
			readIndex_++;
		}


		/// <summary>
		/// 先頭の要素を取得する
		/// </summary>
		/// <returns></returns>
		T& Front() &
		{
			return  buffer_[readIndex_ & (buffer_.size() - 1)] ;
		}

		/// <summary>
		/// 先頭の要素を取得する
		/// </summary>
		/// <returns></returns>
		const T& Front() const &
		{
			return buffer_[readIndex_ & (buffer_.size() - 1)];
		}

		/// <summary>
		/// 末尾の要素を取得する
		/// </summary>
		/// <returns></returns>
		T& Back() &
		{
			return buffer_[(writeIndex_ - 1) & (buffer_.size() - 1)];
		}

		/// <summary>
		/// 末尾の要素を取得する
		/// </summary>
		/// <returns></returns>
		const T& Back() const &
		{
			return buffer_[(writeIndex_ - 1) & (buffer_.size() - 1)];
		}

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		uint64 Size() const
		{
			return std::min(writeIndex_ - readIndex_, buffer_.size());
		}

		/// <summary>
		/// 先頭のイテレーターを取得する
		/// </summary>
		/// <returns></returns>
		iterator begin()
		{
			return { buffer_,0 , readIndex_};
		}

		/// <summary>
		/// 末端のイテレーターを取得する
		/// </summary>
		/// <returns></returns>
		iterator end()
		{
			return { buffer_,buffer_.size(),readIndex_};
		}

		/// <summary>
		/// 先頭のconstイテレーターを取得する
		/// </summary>
		/// <returns></returns>
		const_iterator cbegin() const
		{
			return { buffer_,0 , readIndex_ };
		}

		/// <summary>
		/// 末端のconstイテレーターを取得する
		/// </summary>
		/// <returns></returns>
		const_iterator cend() const
		{
			return { buffer_,buffer_.size(),readIndex_ };
		}
	private:

		/// <summary>
		/// 配列
		/// </summary>
		Array<T, SizeType> buffer_;

		/// <summary>
		/// 書き込みインデックス
		/// </summary>
		SizeType writeIndex_;

		/// <summary>
		/// 読み込みインデックス
		/// </summary>
		SizeType readIndex_;
	};

	/// <summary>
	/// 固定配列版リングバッファ
	/// </summary>
	/// <typeparam name="T"> 要素の型</typeparam>
	/// <typeparam name="size"> サイズ </typeparam>
	template<class T, std::uint64_t size>
	class RingBufferArray
	{
	public:
		class iterator
		{
		public:
			iterator() :
				index_{ 0ull }, startIndex_{ 0ull }
			{
			}

			iterator(std::span<T> span, std::uint64_t index, std::uint64_t startIndex) :
				span_{ span }, index_{ index }, startIndex_{ startIndex }
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
			std::span<T,size> span_;
			std::uint64_t index_;
			std::uint64_t startIndex_;
		};

		RingBufferArray() :
			writeIndex_{ 0ull }, readIndex_{ 0ull }
		{
		}

		RingBufferArray(std::initializer_list<T> initList) :
			buffer_{initList}, writeIndex_{ initList.size() }, readIndex_{ 0ull }
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

		void Emplace(T&& value)
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
			return  buffer_[readIndex_ & (buffer_.size() - 1ull)];
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
			return { buffer_,0ull , readIndex_ };
		}

		iterator end()
		{
			return { buffer_,buffer_.size(),readIndex_ };
		}
	private:

		std::array < T, size> buffer_;
		std::uint64_t writeIndex_;
		std::uint64_t readIndex_;
		static_assert((size& (size - 1)) == 0 && size != 0,"サイズが０以外かつ2の倍数にしてください");
	};

	/// <summary>
	/// ロックフリー版リングバッファ
	/// </summary>
	/// <typeparam name="T"> 要素の型 </typeparam>
	/// <typeparam name="cashLineSize"> キャッシュラインサイズ </typeparam>
	template<class T, std::uint8_t cashLineSize =
#ifdef USE_WINDOWS
	        std::hardware_destructive_interference_size
#else
        64
#endif
        >
	class LockFreeRingBuffer
	{
	public:

		/// <summary>
		/// イテレーター
		/// </summary>
		class iterator
		{
		public:
			iterator()
			{
			}

			iterator(std::span<T> span, std::uint64_t index, std::uint64_t startIndex) :
				span_{ span }, index_{ index }, startIndex_{ startIndex }
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


		LockFreeRingBuffer(std::uint64_t size) :
			buffer_(0), writeIndex_{ 0ull }, readIndex_{ 0ull }
		{
			buffer_.resize(1ull << static_cast<std::uint64_t>(std::ceil(std::log2(size))));
		}

		LockFreeRingBuffer(std::initializer_list<T> initList) :
			buffer_(initList), writeIndex_{ initList.size() }, readIndex_{ 0ull }
		{

		}

		/// <summary>
		/// 要素を変更する
		/// </summary>
		/// <param name="size"></param>
		void Resize(std::uint64_t size)
		{
			size = 1ull << static_cast<std::uint64_t>(std::ceil(std::log2(size)));
			auto nowSize = Size();
			auto readIndex{ readIndex_.load(std::memory_order_acquire) };
			auto writeIndex{ writeIndex_.load(std::memory_order_acquire) };
			for (uint64_t i = 0ull; i < nowSize - 1ull; i++)
			{
				std::swap(buffer_[i], buffer_[(readIndex + i) & (buffer_.size() - 1ull)]);
			}

			readIndex_.store(0ull, std::memory_order_release);
			writeIndex_.store(std::min(nowSize, size), std::memory_order_release);
			buffer_.resize(size);
		}

		void Push(const T& value)
		{
			auto writeIndex{ writeIndex_.load(std::memory_order_relaxed) };

			auto readIndex{ readIndex_.load(std::memory_order_acquire) };
			if (writeIndex - readIndex >= buffer_.size())
			{
				readIndex_.fetch_add(1ull,std::memory_order_release);
			}
			buffer_[writeIndex & (buffer_.size() - 1ull)] = value;

			// ���Z����
			writeIndex_.fetch_add(1ull,std::memory_order_release);
		}

		void Emplace(T&& value)
		{
			auto writeIndex{ writeIndex_.load(std::memory_order_relaxed) };

			auto readIndex{ readIndex_.load(std::memory_order_acquire) };
			if (writeIndex - readIndex >= buffer_.size())
			{
				readIndex_.fetch_add(1ull, std::memory_order_release);
			}
			buffer_[writeIndex & (buffer_.size() - 1ull)] = std::move(value);

			writeIndex_.fetch_add(1ull, std::memory_order_release);
		}

		void Pop()
		{
			readIndex_.fetch_add(1ull, std::memory_order_release);
		}

		T& Front()
		{
			return  buffer_[readIndex_.load(std::memory_order_relaxed) & (buffer_.size() - 1ull)];
		}

		const T& Front() const
		{
			return buffer_.at(readIndex_.load(std::memory_order_relaxed) & (buffer_.size() - 1ull));
		}

		T& Back()
		{
			return buffer_[(writeIndex_.load(std::memory_order_relaxed) - 1u) & (buffer_.size() - 1ull)];
		}

		const T& Back() const
		{
			return buffer_.at((writeIndex_.load(std::memory_order_relaxed) - 1u) & (buffer_.size() - 1ull));
		}

		uint64 Size() const
		{
			return std::min(writeIndex_.load(std::memory_order_relaxed) - readIndex_.load(std::memory_order_relaxed), buffer_.size());
		}


		iterator begin()
		{
			return { buffer_,0ull , readIndex_.load(std::memory_order_relaxed)};
		}

		iterator end()
		{
			return { buffer_,buffer_.size(),readIndex_.load(std::memory_order_relaxed)};
		}
	private:
		std::vector<T> buffer_;
		alignas(cashLineSize) std::atomic_uint64_t readIndex_;
		alignas(cashLineSize) std::atomic_uint64_t writeIndex_;
	};

	/// <summary>
	/// 固定配列版
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <typeparam name="cashLineSize"></typeparam>
	/// <typeparam name="size"></typeparam>
	template<class T, std::uint64_t size,
	        std::uint8_t cashLineSize =
#ifdef USE_WINDOWS
            std::hardware_destructive_interference_size
#else
            64
#endif
                    >
	class LockFreeRingBufferArray
	{
	public:

		/// <summary>
		/// イテレーター
		/// </summary>
		class iterator
		{
		public:
			iterator()
			{
			}

			iterator(std::span<T> span, std::uint64_t index, std::uint64_t startIndex) :
				span_{ span }, index_{ index }, startIndex_{ startIndex }
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
			std::span<T,size> span_;
			std::uint64_t index_;
			std::uint64_t startIndex_;
		};

		LockFreeRingBufferArray(std::initializer_list<T> initList) :
			buffer_(initList), writeIndex_{ initList.size() }, readIndex_{ 0ull }
		{

		}

		void Push(const T& value)
		{
			auto writeIndex{ writeIndex_.load(std::memory_order_relaxed) };

			auto readIndex{ readIndex_.load(std::memory_order_acquire) };
			if (writeIndex - readIndex >= buffer_.size())
			{
				readIndex_.fetch_add(1ull, std::memory_order_release);
			}
			buffer_[writeIndex & (buffer_.size() - 1ull)] = value;

			// ���Z����
			writeIndex_.fetch_add(1ull, std::memory_order_release);
		}

		void Emplace(T&& value)
		{
			auto writeIndex{ writeIndex_.load(std::memory_order_relaxed) };

			auto readIndex{ readIndex_.load(std::memory_order_acquire) };
			if (writeIndex - readIndex >= buffer_.size())
			{
				readIndex_.fetch_add(1ull, std::memory_order_release);
			}
			buffer_[writeIndex & (buffer_.size() - 1ull)] = std::move(value);

			writeIndex_.fetch_add(1ull, std::memory_order_release);
		}

		void Pop()
		{
			readIndex_.fetch_add(1ull, std::memory_order_release);
		}

		T& Front()
		{
			return  buffer_[readIndex_.load(std::memory_order_relaxed) & (buffer_.size() - 1ull)];
		}

		const T& Front() const
		{
			return buffer_.at(readIndex_.load(std::memory_order_relaxed) & (buffer_.size() - 1ull));
		}

		T& Back()
		{
			return buffer_[(writeIndex_.load(std::memory_order_relaxed) - 1u) & (buffer_.size() - 1ull)];
		}

		const T& Back() const
		{
			return buffer_.at((writeIndex_.load(std::memory_order_relaxed) - 1u) & (buffer_.size() - 1ull));
		}

		uint64 Size() const
		{
			return std::min(writeIndex_.load(std::memory_order_relaxed) - readIndex_.load(std::memory_order_relaxed), buffer_.size());
		}


		iterator begin()
		{
			return { buffer_,0ull , readIndex_.load(std::memory_order_relaxed) };
		}

		iterator end()
		{
			return { buffer_,buffer_.size(),readIndex_.load(std::memory_order_relaxed) };
		}
	private:
		std::array<T,size> buffer_;
		alignas(cashLineSize) std::atomic_uint64_t readIndex_;
		alignas(cashLineSize) std::atomic_uint64_t writeIndex_;
		static_assert((size& (size - 1)) == 0 && size != 0, "サイズが０以外かつ2の倍数にしてください");
	};
}

