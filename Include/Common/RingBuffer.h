#pragma once
#include <vector>
#include <array>
#include <atomic>
#include "../Math/Geometry.h"

namespace Eugene
{
	/// <summary>
	/// リサイズ可能なリングバッファ、要素数は2の冪乗固定になる
	/// </summary>
	/// <typeparam name="T"> 要素の型 </typeparam>
	template<class T>
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

			/// <summary>
			/// 元のバッファを参照するためのspan
			/// </summary>
			std::span<T> span_;

			
			/// <summary>
			/// 現在のインデックス
			/// </summary>
			std::uint64_t index_;

			/// <summary>
			/// 開始インデックス
			/// </summary>
			std::uint64_t startIndex_;
		};

		/// <summary>
		/// サイズ指定ができるコンストラクタ
		/// </summary>
		/// <param name="size"> 要素数 </param>
		RingBuffer(std::uint64_t size) :
			buffer_(0), writeIndex_{ 0ull }, readIndex_{ 0ull }
		{
			buffer_.resize(1ull << static_cast<std::uint64_t>(std::ceil(std::log2(size))));
		}

		RingBuffer(std::initializer_list<T> initList) :
			buffer_(initList), writeIndex_{ initList.size()}, readIndex_{0ull}
		{

		}


		/// <summary>
		/// 要素数を変更する
		/// </summary>
		/// <param name="size"></param>
		void Resize(std::uint64_t size)
		{
			size = 1ull << static_cast<std::uint64_t>(std::ceil(std::log2(size)));
			auto nowSize = Size();
			for (uint64_t i = 0ull; i < nowSize - 1ull; i++)
			{
				std::swap(buffer_[i], buffer_[(readIndex_ + i) & (buffer_.size() - 1ull)]);
			}
			
			readIndex_ = 0ull;
			writeIndex_ = std::min(nowSize, size);
			buffer_.resize(size);
		}

		/// <summary>
		/// コピーした要素を追加する
		/// </summary>
		/// <param name="value"></param>
		void Push(const T& value)
		{
			if (writeIndex_ - readIndex_ >= buffer_.size()) 
			{
				readIndex_++;
			}
			buffer_[writeIndex_++ & (buffer_.size() - 1ull)] = value;
		}
		
		/// <summary>
		/// 要素を追加する
		/// </summary>
		/// <param name="value"></param>
		void Emplace(T&&value)
		{
			if (writeIndex_ - readIndex_ >= buffer_.size())
			{
				readIndex_++;
			}
			buffer_[writeIndex_++ & (buffer_.size() - 1ull)] = std::move(value);
		}

		/// <summary>
		/// 要素を削除する
		/// </summary>
		void Pop()
		{
			readIndex_++;
		}


		/// <summary>
		/// 先頭の参照を取得する
		/// </summary>
		/// <returns></returns>
		T& Front()
		{
			return  buffer_[readIndex_ & (buffer_.size() - 1ull)] ;
		}

		/// <summary>
		/// 先頭の参照を取得する
		/// </summary>
		/// <returns></returns>
		const T& Front() const
		{
			return buffer_.at(readIndex_ & (buffer_.size() - 1ull));
		}

		/// <summary>
		/// 末尾の参照を取得する
		/// </summary>
		/// <returns></returns>
		T& Back()
		{
			return buffer_[(writeIndex_ - 1u) & (buffer_.size() - 1ull)];
		}

		/// <summary>
		/// 末尾を取得する
		/// </summary>
		/// <returns></returns>
		const T& Back() const
		{
			return buffer_.at((writeIndex_ - 1u) & (buffer_.size() - 1ull));
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
			return { buffer_,0ull , readIndex_};
		}

		/// <summary>
		/// 末端のイテレーターを取得する
		/// </summary>
		/// <returns></returns>
		iterator end()
		{
			return { buffer_,buffer_.size(),readIndex_};
		}
	private:

		/// <summary>
		/// バッファ
		/// </summary>
		std::vector<T> buffer_;

		/// <summary>
		/// 書き込み位置のインデックス
		/// </summary>
		std::uint64_t writeIndex_;

		/// <summary>
		/// 読み取り位置のインデックス
		/// </summary>
		std::uint64_t readIndex_;
	};

	/// <summary>
	/// リングバッファの固定サイズ版
	/// </summary>
	/// <typeparam name="T"> 要素の型 </typeparam>
	/// <typeparam name="size"> 要素の数(2のべき乗の数) </typeparam>
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

		RingBufferArray(std::uint64_t size) :
			buffer_(0), writeIndex_{ 0ull }, readIndex_{ 0ull }
		{
			buffer_.resize(AlignmentedSize(size, 2ull));
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
		static_assert((size& (size - 1)) == 0 && size != 0,"2の冪乗のサイズを指定してください");
	};

	/// <summary>
	/// ロックフリー版リングバッファ
	/// </summary>
	/// <typeparam name="T"> 要素の型 </typeparam>
	/// <typeparam name="cashLineSize"> キャッシュラインのサイズ(CPUのやつに合わせて) </typeparam>
	template<class T, std::uint8_t cashLineSize = 64>
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
		/// 要素数を変更する
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
			// 順序関係なしで読み込む
			auto writeIndex{ writeIndex_.load(std::memory_order_relaxed) };

			// 読み込みをする
			auto readIndex{ readIndex_.load(std::memory_order_acquire) };
			if (writeIndex - readIndex >= buffer_.size())
			{
				readIndex_.fetch_add(1ull,std::memory_order_release);
			}
			buffer_[writeIndex & (buffer_.size() - 1ull)] = value;

			// 加算する
			writeIndex_.fetch_add(1ull,std::memory_order_release);
		}

		void Emplace(T&& value)
		{
			// 順序関係なしで読み込む
			auto writeIndex{ writeIndex_.load(std::memory_order_relaxed) };

			// 読み込みをする
			auto readIndex{ readIndex_.load(std::memory_order_acquire) };
			if (writeIndex - readIndex >= buffer_.size())
			{
				readIndex_.fetch_add(1ull, std::memory_order_release);
			}
			buffer_[writeIndex & (buffer_.size() - 1ull)] = std::move(value);

			// 加算する
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
	/// ロックフリー版リングバッファの固定サイズ
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <typeparam name="cashLineSize"></typeparam>
	/// <typeparam name="size"></typeparam>
	template<class T, std::uint64_t size,std::uint8_t cashLineSize = 64>
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
			// 順序関係なしで読み込む
			auto writeIndex{ writeIndex_.load(std::memory_order_relaxed) };

			// 読み込みをする
			auto readIndex{ readIndex_.load(std::memory_order_acquire) };
			if (writeIndex - readIndex >= buffer_.size())
			{
				readIndex_.fetch_add(1ull, std::memory_order_release);
			}
			buffer_[writeIndex & (buffer_.size() - 1ull)] = value;

			// 加算する
			writeIndex_.fetch_add(1ull, std::memory_order_release);
		}

		void Emplace(T&& value)
		{
			// 順序関係なしで読み込む
			auto writeIndex{ writeIndex_.load(std::memory_order_relaxed) };

			// 読み込みをする
			auto readIndex{ readIndex_.load(std::memory_order_acquire) };
			if (writeIndex - readIndex >= buffer_.size())
			{
				readIndex_.fetch_add(1ull, std::memory_order_release);
			}
			buffer_[writeIndex & (buffer_.size() - 1ull)] = std::move(value);

			// 加算する
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
		static_assert((size& (size - 1)) == 0 && size != 0, "2の冪乗のサイズを指定してください");
	};
}

