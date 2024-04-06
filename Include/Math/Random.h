#pragma once
#include <random>
#include <concepts>
#include <algorithm>

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
	/// 64�r�b�g��xor�V�t�g�̗��������G���W��
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
	/// �����������N���X
	/// </summary>
	/// <typeparam name="R"> �����G���W���̌^ </typeparam>
	template<RandomEngine R>
	class Random
	{
	public:
		Random()
		{
			engine_.seed(std::random_device()());
		}

		/// <summary>
		/// p�̊m����true��Ԃ�
		/// </summary>
		/// <param name="p"> true�ɂȂ�m�� </param>
		/// <returns></returns>
		bool operator()(double p) noexcept
		{
			std::bernoulli_distribution d(std::clamp(p, 0.0, 1.0));
			return d(engine_);
		}
		
		/// <summary>
		/// �w��̐����l�͈̔͂Œl��Ԃ�
		/// </summary>
		/// <typeparam name="Int"> �����̌^ </typeparam>
		/// <param name="min"> �ŏ��l </param>
		/// <param name="max"> �ő�l </param>
		/// <returns> ���� </returns>
		template<std::integral Int>
		Int operator()(Int min, Int max) noexcept
		{
			std::uniform_int_distribution<Int> d(min, max);
			return d(engine_);
		}

		/// <summary>
		/// �w��̕��������_���͈̔͂Œl��Ԃ�
		/// </summary>
		/// <typeparam name="Float"> ���������_���̌^ </typeparam>
		/// <param name="min"> �ŏ��l </param>
		/// <param name="max"> �ő�l </param>
		/// <returns> ���� </returns>
		template<std::floating_point Float>
		Float operator()(Float min, Float max) noexcept
		{
			std::uniform_real_distribution<Float> d(min, max);
			return d(engine_);
		}

		/// <summary>
		/// �w��̊m�����w��̉񐔍s�����ꍇ�̐�����
		/// </summary>
		/// <typeparam name="Int"> �񐔂̐����^ </typeparam>
		/// <param name="count"> ���s�� </param>
		/// <param name="p"> �m�� </param>
		/// <returns></returns>
		template<std::integral Int>
		Int operator()(Int count, double p) noexcept
		{
			std::binomial_distribution<> d(count, p);
			return d(engine_);
		}

		/// <summary>
		/// ������Ԃ�
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