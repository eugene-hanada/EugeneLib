#pragma once
#include <shared_mutex>

namespace Eugene
{
	/// <summary>
	/// �����N�ŋ��L�|�C���^���Ǘ�����X�}�[�g�|�C���^
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<class T>
	class SharedPtr
	{
	public:

		/// <summary>
		/// �f�t�H���g�R���X�g���N�^
		/// </summary>
		SharedPtr():
			next_{nullptr},prev_{nullptr},ptr_{nullptr}
		{
		}

		/// <summary>
		/// �Ǘ��Ώۃ|�C���^���󂯎��R���X�g���N�^
		/// </summary>
		/// <param name="ptr"></param>
		SharedPtr(T* ptr):
			next_{ nullptr }, prev_{ nullptr }, ptr_{ ptr }
		{
		}


		/// <summary>
		/// �Q�Ƃ���R�s�[����R���X�g���N�^
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
		/// ���[�u�R���X�g���N�^
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
		/// �f�X�g���N�^
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
		/// �������
		/// </summary>
		void Release()
		{
			if (next_ == nullptr && prev_ == nullptr)
			{
				// �O�オnullptr�̎��͎��g���Ō�̎Q�ƂȂ̂�delete
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
		/// �Ǘ��Ώۂ̃|�C���^��ݒ肵�Ȃ���
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
		/// ���̃|�C���^���擾����
		/// </summary>
		/// <returns></returns>
		const SharedPtr* GetNext()const
		{
			return next_;
		}

		/// <summary>
		/// ���̃|�C���^���Z�b�g����
		/// </summary>
		/// <param name="sharedPtr"></param>
		void SetNext(const SharedPtr* sharedPtr)
		{
			next_ = sharedPtr;
		}

		/// <summary>
		/// �O�̃|�C���^���擾����
		/// </summary>
		/// <returns></returns>
		const SharedPtr* GetPrev()const
		{
			return prev_;
		}

		/// <summary>
		/// �O�̃|�C���^���Z�b�g����
		/// </summary>
		/// <param name="sharedPtr"></param>
		void SetPrev(const SharedPtr* sharedPtr)
		{
			prev_ = sharedPtr;
		}

		/// <summary>
		/// �|�C���^���擾����
		/// </summary>
		/// <returns></returns>
		const T* GetPtr()const
		{
			return ptr_;
		}


		/// <summary>
		/// �|�C���^���Z�b�g����
		/// </summary>
		/// <param name="ptr"></param>
		void SetPtr(const T* ptr)
		{
			ptr_ = ptr;
		}

		/// <summary>
		/// ���ւ̃|�C���^
		/// </summary>
		SharedPtr* next_;

		/// <summary>
		/// �O�̃|�C���^
		/// </summary>
		SharedPtr* prev_;

		/// <summary>
		/// �Ǘ��Ώۂ̃|�C���^
		/// </summary>
		T* ptr_;
	};

	template<class T,class ...Args>
	SharedPtr<T> Make_Shared(Args... args)
	{
		return { new T{args...} };
	}

	/// <summary>
	/// �����N�ŋ��L�|�C���^���Ǘ�����X�}�[�g�|�C���^�̃X���b�h�Z�[�t��
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<class T>
	class ThreadSafeSharedPtr
	{
	public:

		/// <summary>
		/// �R���X�g���N�^
		/// </summary>
		ThreadSafeSharedPtr() :
			next_{ nullptr }, prev_{ nullptr }, ptr_{ nullptr }
		{
		}

		/// <summary>
		/// �w��̃|�C���^���Ǘ�����R���X�g���N�^
		/// </summary>
		/// <param name="ptr"></param>
		ThreadSafeSharedPtr(T* ptr) :
			next_{ nullptr }, prev_{ nullptr }, ptr_{ ptr }
		{
		}

		/// <summary>
		/// �Q�Ƃ���R�s�[���s���R���X�g���N�^
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
		/// ���[�u�R���X�g���N�^
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
		/// �f�X�g���N�^
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
		/// �������
		/// </summary>
		void Release()
		{
			Lock();
			if (next_ == nullptr && prev_ == nullptr)
			{
				// �O�オnullptr�̎��͎��g���Ō�̎Q�ƂȂ̂�delete
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
		/// �Ǘ��Ώۂ̃|�C���^���Đݒ肷��
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
		/// ���ւ̃|�C���^
		/// </summary>
		/// <returns></returns>
		const ThreadSafeSharedPtr* GetNext()const
		{
			return next_;
		}

		/// <summary>
		/// ���ւ̃|�C���^���Z�b�g����
		/// </summary>
		/// <param name="sharedPtr"></param>
		void SetNext(const ThreadSafeSharedPtr* sharedPtr)
		{
			next_ = sharedPtr;
		}

		/// <summary>
		/// �O�ւ̃|�C���^
		/// </summary>
		/// <returns></returns>
		const ThreadSafeSharedPtr* GetPrev()const
		{
			return prev_;
		}

		/// <summary>
		/// �O�̃|�C���^�փZ�b�g����
		/// </summary>
		/// <param name="sharedPtr"></param>
		void SetPrev(const ThreadSafeSharedPtr* sharedPtr)
		{
			prev_ = sharedPtr;
		}


		/// <summary>
		/// �|�C���^���擾����
		/// </summary>
		/// <returns></returns>
		const T* GetPtr()const
		{
			return ptr_;
		}

		/// <summary>
		/// �|�C���^���Z�b�g����
		/// </summary>
		/// <param name="ptr"></param>
		void SetPtr(const T* ptr)
		{
			ptr_ = ptr;
		}
		
		/// <summary>
		/// ���b�N����
		/// </summary>
		void Lock()
		{
			mutex_.lock();
		}

		/// <summary>
		/// �A�����b�N����
		/// </summary>
		void UnLock()
		{
			mutex_.unlock();
		}

		/// <summary>
		/// ���L���b�N����
		/// </summary>
		void SharedLock()
		{
			mutex_.lock_shared();
		}

		/// <summary>
		/// �A�����b�N����
		/// </summary>
		void SharedUnLock()
		{
			mutex_.unlock_shared();
		}

		/// <summary>
		/// �~���[�e�b�N�X
		/// </summary>
		std::shared_mutex mutex_;

		/// <summary>
		///���ւ̃|�C���^
		/// </summary>
		ThreadSafeSharedPtr* next_;

		/// <summary>
		/// �O�ւ̃|�C���^
		/// </summary>
		ThreadSafeSharedPtr* prev_;

		/// <summary>
		/// �Ǘ��Ώۂ̃|�C���^
		/// </summary>
		T* ptr_;
	};

	template<class T, class ...Args>
	ThreadSafeSharedPtr<T> Make_ThreadSafeShared(Args... args)
	{
		return { new T{args...} };
	}
}
