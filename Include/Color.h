#pragma once
#include <array>
#include <algorithm>
#include "Math/MathConcepts.h"

namespace Eugene
{
	template<ValueC T, std::uint64_t Num, T MaxValue = static_cast<T>(1), T MinValue = static_cast<T>(0)>
	class Color
	{
	public:
		constexpr Color()
		{
			color_.fill(MaxValue);
		}

		constexpr Color(T value)
		{
			color_.fill(value);
		}

		constexpr Color(std::span<T, Num> color)
		{
			for (std::uint64_t i = 0; i < Num; i++)
			{
				color_[i] = color[i];
			}
		}

		constexpr void SetValue(std::uint64_t idx, const T& value)
		{
			color_[idx] = std::clamp(value, MinValue, MaxValue);
		}

		const std::array<T, Num>& ToArray(void) const&
		{
			return color_;
		}

		constexpr std::uint64_t GetSize(void) const
		{
			return Num;
		}

		constexpr std::uint64_t GetMax(void) const
		{
			return MaxValue;
		}
		constexpr std::uint64_t GetMin(void) const
		{
			return MinValue;
		}

		const T& operator[](std::uint64_t idx) 
		{
			return color_[idx];
		}

	private:
		std::array<T, Num> color_;
	};

	using ColorRGBA = Color<float, 4>;
	using ColorRGB = Color<float, 3>;


}
