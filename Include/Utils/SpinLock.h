#pragma once
#include <atomic>
#include <thread>

namespace Eugene
{
	class SpinLock
	{
	public:
		SpinLock() noexcept : flag_(ATOMIC_FLAG_INIT) {}

		void lock() noexcept
		{
			while (flag_.test_and_set(std::memory_order_acquire))
			{
				std::this_thread::yield(); // ‘¼‚ÌƒXƒŒƒbƒh‚ÉCPU‚ğ÷‚é
			}
		}

		void unlock() noexcept
		{
			flag_.clear(std::memory_order_release);
		}

		bool try_lock() noexcept
		{
			return !flag_.test_and_set(std::memory_order_acquire);
		}

	private:
		std::atomic_flag flag_;
	};
}
