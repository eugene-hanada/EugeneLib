#pragma once
#include <array>
#include <algorithm>
#include "Math/MathConcepts.h"

namespace EugeneLib
{
	template<ValueC T, size_t Num, T MaxValue = static_cast<T>(1), T MinValue = static_cast<T>(0)>
	class Color
	{
	public:
		Color()
		{
			color_.fill(MaxValue);
		}

		void SetValue(size_t idx, const T& value)
		{
			color_[idx] = std::clamp(value, MinValue, MaxValue);
		}

		const std::array<T, Num>& ToArray(void) const&
		{
			return color_;
		}

		constexpr size_t GetSize(void) const
		{
			return Num;
		}

		constexpr size_t GetMax(void) const
		{
			return MaxValue;
		}
		constexpr size_t GetMin(void) const
		{
			return MinValue;
		}

		const T& operator[](size_t idx)
		{
			return color_[idx];
		}

	private:
		std::array<T, Num> color_;
	};

	using ColorRGBA = Color<float, 4>;
	using ColorRGB = Color<float, 3>;


}
