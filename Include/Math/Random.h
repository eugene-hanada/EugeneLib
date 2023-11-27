#pragma once
#include <random>
#include <concepts>

namespace Eugene
{
	template<class T>
	concept RandomEngine = requires (T & e) {
		{ e() } -> std::same_as<std::uint64_t>;
		{ T::max() } -> std::same_as<std::uint64_t>;
		{ T::min() } -> std::same_as<std::uint64_t>;
	}||requires (T& e, std::uint64_t seed) {
		{ e.seed(seed) } -> std::same_as<void>;
	};

	/// <summary>
	/// 64ビット版xorシフトの乱数生成エンジン
	/// </summary>
	class XorShift64Engine
	{
	public:
		using result_type = std::uint64_t;

		result_type operator()();
		static constexpr std::uint64_t max()
		{
			return std::numeric_limits<result_type>::max() - 1ull;
		}
		static constexpr std::uint64_t min()
		{
			return 0ull;
		}
		void seed(std::uint64_t seed);
	private:
		std::uint64_t state_;
	};

	class Xoshiro128ppEngine
	{
	public:
		using result_type = std::uint64_t;
		result_type operator()();
		static constexpr std::uint64_t max()
		{
			return std::numeric_limits<result_type>::max() - 1ull;
		}
		static constexpr std::uint64_t min()
		{
			return 0ull;
		}
		void seed(std::uint64_t seed);
	private:
		std::uint64_t state_[2];
	};

	/// <summary>
	/// 乱数を扱うクラス
	/// </summary>
	/// <typeparam name="R"> 乱数エンジンの型 </typeparam>
	template<RandomEngine R>
	class Random
	{
	public:
		Random()
		{
			engine_.seed(std::random_device()());
		}

		/// <summary>
		/// pの確率でtrueを返す
		/// </summary>
		/// <param name="p"> trueになる確率 </param>
		/// <returns></returns>
		bool operator()(double p) noexcept
		{
			std::bernoulli_distribution d(std::clamp(p, 0.0, 1.0));
			return d(engine_);
		}
		
		/// <summary>
		/// 指定の整数値の範囲で値を返す
		/// </summary>
		/// <typeparam name="Int"> 整数の型 </typeparam>
		/// <param name="min"> 最小値 </param>
		/// <param name="max"> 最大値 </param>
		/// <returns> 結果 </returns>
		template<std::integral Int>
		Int operator()(Int min, Int max) noexcept
		{
			std::uniform_int_distribution<Int> d(min, max);
			return d(engine_);
		}

		/// <summary>
		/// 指定の浮動小数点数の範囲で値を返す
		/// </summary>
		/// <typeparam name="Float"> 浮動小数点数の型 </typeparam>
		/// <param name="min"> 最小値 </param>
		/// <param name="max"> 最大値 </param>
		/// <returns> 結果 </returns>
		template<std::floating_point Float>
		Float operator()(Float min, Float max) noexcept
		{
			std::uniform_real_distribution<Float> d(min, max);
			return d(engine_);
		}

		/// <summary>
		/// 指定の確率を指定の回数行った場合の成功数
		/// </summary>
		/// <typeparam name="Int"> 回数の整数型 </typeparam>
		/// <param name="count"> 試行回数 </param>
		/// <param name="p"> 確率 </param>
		/// <returns></returns>
		template<std::integral Int>
		Int operator()(Int count, double p) noexcept
		{
			std::binomial_distribution<> d(count, p);
			return d(engine_);
		}

		/// <summary>
		/// 乱数を返す
		/// </summary>
		/// <returns></returns>
		std::uint64_t operator()() noexcept
		{
			return engine_();
		}

	private:
		R engine_;
	};

	using RandomXorShift64 = Random<XorShift64Engine>;
	using RandomXoshiro128pp = Random< Xoshiro128ppEngine>;
	using RandomMt = Random<std::mt19937_64>;
}