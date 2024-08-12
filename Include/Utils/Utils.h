#pragma once
#include <bit>
#include <memory>

namespace Eugene
{
	/// <summary>
	/// アライメントされた大きさを返す
	/// </summary>
	/// <param name="size"> サイズ </param>
	/// <param name="alignment"> アライメントサイズ </param>
	/// <returns> アライメントされたサイズ </returns>
	constexpr size_t AlignmentedSize(const size_t& size, const size_t& alignment) noexcept
	{
		if ((alignment & ~0xfffffffffffffffd) == 0ull)
		{
			return (size + alignment - 1ull) & ~(alignment - 1ull);
		}
		return (size + alignment - 1ull) - (size % alignment - 1ull);
	}

	/// <summary>
	/// バイト列を並び帰る
	/// </summary>
	/// <param name="data"> バイト列のポインタ </param>
	/// <param name="size"> バイト数 </param>
	constexpr void BytesSwap(std::uint8_t* data, std::uint64_t size)
	{
		for (std::uint64_t i = 0ull; i < size / 2ull; ++i)
		{
			std::swap(data[i], data[size - i - 1ull]);
		}
	}

	/// <summary>
	/// 動的にインスタンスを生成するシングルトンクラスのテンプレート(継承して使う)
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<class T>
	class DynamicSingleton
	{
	public:
		
		/// <summary>
		/// Tのインスタンスを取得する
		/// </summary>
		/// <returns></returns>
		static constexpr T& GetInstance() noexcept
		{
			return *instance_;
		}

		/// <summary>
		/// 生成を行う
		/// </summary>
		/// <typeparam name="...Args"></typeparam>
		/// <param name="...args"></param>
		template<class ...Args>
		static void Create(Args&&... args)
		{
			if (!instance_)
			{
				instance_ = std::make_unique<T>(args...);
			}
		}

		/// <summary>
		/// 破棄を行う
		/// </summary>
		static constexpr void Destroy() noexcept
		{
			if (instance_)
			{
				instance_.reset();
			}
		}

		/// <summary>
		/// 生成済みか？
		/// </summary>
		static constexpr bool IsCreate() noexcept
		{
			return instance_.operator bool();
		}

	protected:

		/// <summary>
		/// インスタンス
		/// </summary>
		static inline std::unique_ptr<T> instance_;
	};
}
