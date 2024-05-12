#pragma once
#include <shared_mutex>

namespace Eugene
{
	/// <summary>
	/// リンクで共有ポインタを管理するスマートポインタ
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<class T>
	class SharedPtr
	{
	public:

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		SharedPtr():
			next_{nullptr},prev_{nullptr},ptr_{nullptr}
		{
		}

		/// <summary>
		/// 管理対象ポインタを受け取るコンストラクタ
		/// </summary>
		/// <param name="ptr"></param>
		SharedPtr(T* ptr):
			next_{ nullptr }, prev_{ nullptr }, ptr_{ ptr }
		{
		}


		/// <summary>
		/// 参照からコピーするコンストラクタ
		/// </summary>
		/// <param name="sharedPtr"></param>
		SharedPtr(SharedPtr& sharedPtr):
			next_{ nullptr }, prev_{ nullptr }, ptr_{ nullptr }
		{
			ptr_ = sharedPtr.GetPtr();

			if (sharedPtr.GetNext() != nullptr)
			{
				auto srcNext = sharedPtr.GetNext();
				srcNext->SetPrev(this);
				next_ = srcNext;			
			}

			sharedPtr.SetNext(this);
			prev_ = &sharedPtr;

		}

		/// <summary>
		/// ムーブコンストラクタ
		/// </summary>
		/// <param name="sharedPtr"></param>
		SharedPtr(SharedPtr&& sharedPtr):
			next_{ nullptr }, prev_{ nullptr }, ptr_{ nullptr }
		{
			ptr_ = sharedPtr.GetPtr();

			if (sharedPtr.GetNext() != nullptr)
			{
				auto srcNext = sharedPtr.GetNext();
				srcNext->SetPrev(this);
				next_ = srcNext;
			}

			prev_ = sharedPtr.GetPrev();


			sharedPtr.SetNext(nullptr);
			sharedPtr.SetPrev(nullptr);
			sharedPtr.SetPtr(nullptr);
		}

		/// <summary>
		/// デストラクタ
		/// </summary>
		~SharedPtr()
		{
			Release();
		}

		SharedPtr& operator=(SharedPtr& sharedPtr)
		{
			if (ptr_ != nullptr)
			{
				Release();
			}

			ptr_ = sharedPtr.GetPtr();

			if (sharedPtr.GetNext() != nullptr)
			{
				auto srcNext = sharedPtr.GetNext();
				srcNext->SetPrev(this);
				next_ = srcNext;
			}

			sharedPtr.SetNext(this);
			prev_ = &sharedPtr;

			return *this;
		}

		SharedPtr& operator=(SharedPtr&& sharedPtr)
		{
			if (ptr_ != nullptr)
			{
				Release();
			}

			ptr_ = sharedPtr.GetPtr();

			if (sharedPtr.GetNext() != nullptr)
			{
				auto srcNext = sharedPtr.GetNext();
				srcNext->SetPrev(this);
				next_ = srcNext;
			}

			prev_ = sharedPtr.GetPrev();


			sharedPtr.SetNext(nullptr);
			sharedPtr.SetPrev(nullptr);
			sharedPtr.SetPtr(nullptr);
		}

		SharedPtr& operator=(const T* ptr)
		{
			Reset(ptr);
		}

		T& operator*()&
		{
			return *ptr_;
		}

		const T* operator->()const
		{
			return ptr_;
		}

		/// <summary>
		/// 解放処理
		/// </summary>
		void Release()
		{
			if (next_ == nullptr && prev_ == nullptr)
			{
				// 前後がnullptrの時は自身が最後の参照なのでdelete
				delete ptr_;
				return;
			}

			if (next_ != nullptr)
			{
				if (prev_ != nullptr)
				{
					next_->SetPrev(prev_);
				}
				else
				{
					next_->SetPrev(nullptr);
				}
			}

			if (prev_ != nullptr)
			{
				if (next_ != nullptr)
				{
					prev_->SetNext(next_);
				}
				else
				{
					prev_->SetNext(nullptr);
				}
			}
		}

		/// <summary>
		/// 管理対象のポインタを設定しなおす
		/// </summary>
		/// <param name="ptr"></param>
		void Reset(const T* ptr = nullptr)
		{
			if (ptr_ != nullptr)
			{
				Release();
			}
			next_ = nullptr;
			prev_ = nullptr;
			ptr_ = ptr;
		}

	private:

		/// <summary>
		/// 次のポインタを取得する
		/// </summary>
		/// <returns></returns>
		const SharedPtr* GetNext()const
		{
			return next_;
		}

		/// <summary>
		/// 次のポインタをセットする
		/// </summary>
		/// <param name="sharedPtr"></param>
		void SetNext(const SharedPtr* sharedPtr)
		{
			next_ = sharedPtr;
		}

		/// <summary>
		/// 前のポインタを取得する
		/// </summary>
		/// <returns></returns>
		const SharedPtr* GetPrev()const
		{
			return prev_;
		}

		/// <summary>
		/// 前のポインタをセットする
		/// </summary>
		/// <param name="sharedPtr"></param>
		void SetPrev(const SharedPtr* sharedPtr)
		{
			prev_ = sharedPtr;
		}

		/// <summary>
		/// ポインタを取得する
		/// </summary>
		/// <returns></returns>
		const T* GetPtr()const
		{
			return ptr_;
		}


		/// <summary>
		/// ポインタをセットする
		/// </summary>
		/// <param name="ptr"></param>
		void SetPtr(const T* ptr)
		{
			ptr_ = ptr;
		}

		/// <summary>
		/// 次へのポインタ
		/// </summary>
		SharedPtr* next_;

		/// <summary>
		/// 前のポインタ
		/// </summary>
		SharedPtr* prev_;

		/// <summary>
		/// 管理対象のポインタ
		/// </summary>
		T* ptr_;
	};

	template<class T,class ...Args>
	SharedPtr<T> Make_Shared(Args... args)
	{
		return { new T{args...} };
	}

	/// <summary>
	/// リンクで共有ポインタを管理するスマートポインタのスレッドセーフ版
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<class T>
	class ThreadSafeSharedPtr
	{
	public:

		/// <summary>
		/// コンストラクタ
		/// </summary>
		ThreadSafeSharedPtr() :
			next_{ nullptr }, prev_{ nullptr }, ptr_{ nullptr }
		{
		}

		/// <summary>
		/// 指定のポインタを管理するコンストラクタ
		/// </summary>
		/// <param name="ptr"></param>
		ThreadSafeSharedPtr(T* ptr) :
			next_{ nullptr }, prev_{ nullptr }, ptr_{ ptr }
		{
		}

		/// <summary>
		/// 参照からコピーを行うコンストラクタ
		/// </summary>
		/// <param name="sharedPtr"></param>
		ThreadSafeSharedPtr(ThreadSafeSharedPtr& sharedPtr) :
			next_{ nullptr }, prev_{ nullptr }, ptr_{ nullptr }
		{
			Lock();
			sharedPtr.Lock();
			ptr_ = sharedPtr.GetPtr();

			if (sharedPtr.GetNext() != nullptr)
			{
				auto srcNext = sharedPtr.GetNext();
				srcNext->SetPrev(this);
				next_ = srcNext;
			}

			sharedPtr.SetNext(this);
			prev_ = &sharedPtr;
			sharedPtr.UnLock();
			UnLock();
		}

		/// <summary>
		/// ムーブコンストラクタ
		/// </summary>
		/// <param name="sharedPtr"></param>
		ThreadSafeSharedPtr(ThreadSafeSharedPtr&& sharedPtr):
			next_{ nullptr }, prev_{ nullptr }, ptr_{ nullptr }
		{
			Lock();
			sharedPtr.Lock();
			ptr_ = sharedPtr.GetPtr();

			if (sharedPtr.GetNext() != nullptr)
			{
				auto srcNext = sharedPtr.GetNext();
				srcNext->SetPrev(this);
				next_ = srcNext;
			}

			prev_ = sharedPtr.GetPrev();
			UnLock();
			sharedPtr.SetNext(nullptr);
			sharedPtr.SetPrev(nullptr);
			sharedPtr.SetPtr(nullptr);
			sharedPtr.UnLock();
			
		}

		/// <summary>
		/// デストラクタ
		/// </summary>
		~ThreadSafeSharedPtr()
		{
			Release();
		}

		ThreadSafeSharedPtr& operator=(ThreadSafeSharedPtr& sharedPtr)
		{
			SharedLock();
			if (ptr_ != nullptr)
			{
				SharedUnLock();
				Release();
			}
			SharedUnLock();

			Lock();
			sharedPtr.Lock();
			ptr_ = sharedPtr.GetPtr();

			if (sharedPtr.GetNext() != nullptr)
			{
				auto srcNext = sharedPtr.GetNext();
				srcNext->SetPrev(this);
				next_ = srcNext;
			}

			sharedPtr.SetNext(this);
			prev_ = &sharedPtr;
			sharedPtr.UnLock();
			UnLock();
			return *this;
		}

		ThreadSafeSharedPtr& operator=(ThreadSafeSharedPtr&& sharedPtr)
		{
			SharedLock();
			if (ptr_ != nullptr)
			{
				SharedUnLock();
				Release();
			}
			SharedUnLock();

			Lock();
			sharedPtr.Lock();
			ptr_ = sharedPtr.GetPtr();

			if (sharedPtr.GetNext() != nullptr)
			{
				auto srcNext = sharedPtr.GetNext();
				srcNext->SetPrev(this);
				next_ = srcNext;
			}

			prev_ = sharedPtr.GetPrev();
			UnLock();
			sharedPtr.SetNext(nullptr);
			sharedPtr.SetPrev(nullptr);
			sharedPtr.SetPtr(nullptr);
			sharedPtr.UnLock();
		}

		ThreadSafeSharedPtr& operator=(const T* ptr)
		{
			Reset(ptr);
		}

		T& operator*()&
		{
			return *ptr_;
		}

		const T* operator->()const
		{
			return ptr_;
		}

		/// <summary>
		/// 解放処理
		/// </summary>
		void Release()
		{
			Lock();
			if (next_ == nullptr && prev_ == nullptr)
			{
				// 前後がnullptrの時は自身が最後の参照なのでdelete
				delete ptr_;
				return;
			}

			if (next_ != nullptr)
			{
				next_->Lock();
				if (prev_ != nullptr)
				{
					prev_->SharedLock();
					next_->SetPrev(prev_);
					prev_->SharedUnLock();
				}
				else
				{
					next_->SetPrev(nullptr);
				}
				next_->UnLock();
			}

			if (prev_ != nullptr)
			{
				prev_->Lock();
				if (next_ != nullptr)
				{
					next_->SharedLock();
					prev_->SetNext(next_);
					next_->SharedUnLock();
				}
				else
				{
					prev_->SetNext(nullptr);
				}
				prev_->UnLock();
			}
			UnLock();
		}

		/// <summary>
		/// 管理対象のポインタを再設定する
		/// </summary>
		/// <param name="ptr"></param>
		void Reset(const T* ptr = nullptr)
		{
			SharedLock();
			if (ptr_ != nullptr)
			{
				SharedUnLock();
				Release();
			}
			SharedUnLock();

			Lock();
			next_ = nullptr;
			prev_ = nullptr;
			ptr_ = ptr;
			UnLock();
		}

	private:

		/// <summary>
		/// 次へのポインタ
		/// </summary>
		/// <returns></returns>
		const ThreadSafeSharedPtr* GetNext()const
		{
			return next_;
		}

		/// <summary>
		/// 次へのポインタをセットする
		/// </summary>
		/// <param name="sharedPtr"></param>
		void SetNext(const ThreadSafeSharedPtr* sharedPtr)
		{
			next_ = sharedPtr;
		}

		/// <summary>
		/// 前へのポインタ
		/// </summary>
		/// <returns></returns>
		const ThreadSafeSharedPtr* GetPrev()const
		{
			return prev_;
		}

		/// <summary>
		/// 前のポインタへセットする
		/// </summary>
		/// <param name="sharedPtr"></param>
		void SetPrev(const ThreadSafeSharedPtr* sharedPtr)
		{
			prev_ = sharedPtr;
		}


		/// <summary>
		/// ポインタを取得する
		/// </summary>
		/// <returns></returns>
		const T* GetPtr()const
		{
			return ptr_;
		}

		/// <summary>
		/// ポインタをセットする
		/// </summary>
		/// <param name="ptr"></param>
		void SetPtr(const T* ptr)
		{
			ptr_ = ptr;
		}
		
		/// <summary>
		/// ロックする
		/// </summary>
		void Lock()
		{
			mutex_.lock();
		}

		/// <summary>
		/// アンロックする
		/// </summary>
		void UnLock()
		{
			mutex_.unlock();
		}

		/// <summary>
		/// 共有ロックする
		/// </summary>
		void SharedLock()
		{
			mutex_.lock_shared();
		}

		/// <summary>
		/// アンロックする
		/// </summary>
		void SharedUnLock()
		{
			mutex_.unlock_shared();
		}

		/// <summary>
		/// ミューテックス
		/// </summary>
		std::shared_mutex mutex_;

		/// <summary>
		///次へのポインタ
		/// </summary>
		ThreadSafeSharedPtr* next_;

		/// <summary>
		/// 前へのポインタ
		/// </summary>
		ThreadSafeSharedPtr* prev_;

		/// <summary>
		/// 管理対象のポインタ
		/// </summary>
		T* ptr_;
	};

	template<class T, class ...Args>
	ThreadSafeSharedPtr<T> Make_ThreadSafeShared(Args... args)
	{
		return { new T{args...} };
	}
}
