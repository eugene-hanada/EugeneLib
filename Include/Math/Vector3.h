#pragma once
#include <cmath>
#include "MathConcepts.h"

namespace Eugene
{

	template<ValueC T>
	class Vector3Tmp
	{
	public:
		T x;
		T y;
		T z;

		constexpr Vector3Tmp() :
			x{ static_cast<T>(0) }, y{ static_cast<T>(0) }, z{ static_cast<T>(0) }
		{
		}

		constexpr Vector3Tmp(T inX, T inY, T inZ) :
			x{ inX }, y{ inY }, z{ inZ }
		{
		}

		constexpr Vector3Tmp(const Vector3Tmp<T>& vec) :
			x{ vec.x }, y{ vec.y }, z{ vec.z }
		{
		}

		T Magnitude(void) const;

		T SqMagnitude(void) const;

		void Normalize(void);

		Vector3Tmp<T> Normalized(void) const;

		/// <summary> 法線ベクトルに対しての反射ベクトルを返す </summary>
		/// <param name="n"> 法線ベクトル(正規化済み) </param>
		/// <returns> 反射ベクトル </returns>
		Vector3Tmp<T> Refrect(const Vector3Tmp<T>& n);


		template<ValueC T2> operator Vector3Tmp<T2>() const;

		Vector3Tmp<T> operator-() const;


		Vector3Tmp<T> operator+() const;

		constexpr Vector3Tmp<T>& operator=(const Vector3Tmp<T> vec)
		{
			x = vec.x;
			y = vec.y;
			z = vec.z;
			return *this;
		}
		constexpr Vector3Tmp<T>& operator=(const T& val)
		{
			x = val;
			y = val;
			z = val;
			return *this;
		}

		Vector3Tmp<T>& operator+=(const Vector3Tmp<T>& vec);
		Vector3Tmp<T>& operator+=(const T& val);

		Vector3Tmp<T>& operator-=(const Vector3Tmp<T>& vec);
		Vector3Tmp<T>& operator-=(const T& val);

		Vector3Tmp<T>& operator*=(const Vector3Tmp<T>& vec);
		Vector3Tmp<T>& operator*=(const T& val);

		Vector3Tmp<T>& operator/=(const Vector3Tmp<T>& vec);
		Vector3Tmp<T>& operator/=(const T& val);

		Vector3Tmp<T>& operator%=(const Vector3Tmp<T>& vec);
		Vector3Tmp<T>& operator%=(const T& val);

	};

	template<ValueC T>
	constexpr Vector3Tmp<T> operator+(const Vector3Tmp<T>& lVec, const Vector3Tmp<T>& rVec)
	{
		/*if constexpr (std::is_floating_point<T>())
		{
			auto result = DirectX::operator+(lVec.ToSimdVector(), rVec.ToSimdVector());
			return { result.m128_f32[0], result.m128_f32[1], result.m128_f32[2] };
		}
		else*/
		{
			return { lVec.x + rVec.x ,lVec.y + rVec.y, lVec.z + rVec.z };
		}
	}

	template<ValueC T>
	constexpr Vector3Tmp<T> operator+(const Vector3Tmp<T>& lVec, const T& val)
	{
		/*if constexpr (std::is_floating_point<T>())
		{
			auto result = DirectX::operator+(lVec.ToSimdVector(), DirectX::XMVectorSet(val, val, val, 0));
			return { result.m128_f32[0], result.m128_f32[1], result.m128_f32[2] };
		}
		else*/
		{
			return { lVec.x + val, lVec.y + val, lVec.z + val };
		}
	}

	template<ValueC T>
	constexpr Vector3Tmp<T> operator-(const Vector3Tmp<T>& lVec, const Vector3Tmp<T>& rVec)
	{

		{
			return { static_cast<T>(lVec.x - rVec.x), static_cast<T>(lVec.y - rVec.y) , static_cast<T>(lVec.z - rVec.z) };
		}
	}

	template<ValueC T>
	constexpr Vector3Tmp<T> operator-(const Vector3Tmp<T>& lVec, const T& val)
	{
		/*if constexpr (std::is_floating_point<T>())
		{
			auto result = DirectX::operator-(lVec.ToSimdVector(), DirectX::XMVectorSet(val, val, val, 0));
			return { result.m128_f32[0], result.m128_f32[1], result.m128_f32[2] };
		}
		else*/
		{
			return { lVec.x - val, lVec.y - val, lVec.z - val };
		}
	}

	template<ValueC T>
	constexpr Vector3Tmp<T> operator*(const Vector3Tmp<T>& lVec, const Vector3Tmp<T>& rVec)
	{
	/*	if constexpr (std::is_floating_point<T>())
		{
			auto result = DirectX::operator*(lVec.ToSimdVector(), rVec.ToSimdVector());
			return { result.m128_f32[0], result.m128_f32[1], result.m128_f32[2] };
		}
		else*/
		{
			return { lVec.x * rVec.x, lVec.y * rVec.y, lVec.z * rVec.z };
		}
	}

	template<ValueC T>
	constexpr Vector3Tmp<T> operator*(const Vector3Tmp<T>& lVec, const T& val)
	{

		{
			return { static_cast<T>(lVec.x * val), static_cast<T>(lVec.y * val), static_cast<T>(lVec.z * val) };
		}
	}

	template<ValueC T>
	constexpr Vector3Tmp<T> operator/(const Vector3Tmp<T>& lVec, const Vector3Tmp<T>& rVec)
	{
		if (rVec.x == static_cast<T>(0) || rVec.y == static_cast<T>(0))
		{
			return lVec;
		}
		/*if constexpr (std::is_floating_point<T>())
		{
			auto result = DirectX::operator/(lVec.ToSimdVector(), rVec.ToSimdVector());
			return { result.m128_f32[0], result.m128_f32[1], result.m128_f32[2] };
		}*/
		//else
		{
			return { lVec.x / rVec.x, lVec.y / rVec.y , lVec.z / rVec.z };
		}
	}

	template<ValueC T>
	constexpr Vector3Tmp<T> operator/(const Vector3Tmp<T>& lVec, const T& val)
	{
		if (val == static_cast<T>(0))
		{
			return lVec;
		}
		/*if constexpr (std::is_floating_point<T>())
		{
			auto result = DirectX::operator/(lVec.ToSimdVector(), DirectX::XMVectorSet(val, val, val, 0));
			return { result.m128_f32[0], result.m128_f32[1], result.m128_f32[2] };
		}*/
		//else
		{
			return { lVec.x / val, lVec.y / val, lVec.z / val };
		}
	}

	template<ValueC T>
	constexpr Vector3Tmp<T> operator%(const Vector3Tmp<T>& lVec, const Vector3Tmp<T>& rVec)
	{
		if (rVec.x == static_cast<T>(0) || rVec.y == static_cast<T>(0))
		{
			return lVec;
		}

		/*if constexpr (std::is_floating_point<T>())
		{
			auto result = DirectX::XMVectorMod / (lVec.ToSimdVector(), rVec.ToSimdVector());
			return { result.m128_f32[0], result.m128_f32[1], result.m128_f32[2] };
		}*/
		//else
		{
			return { std::fmod(lVec.x , rVec.x), std::fmod(lVec.y , rVec.y), std::fmod(lVec.z , rVec.z) };
		}
	}
	template<ValueC T>
	constexpr Vector3Tmp<T> operator%(const Vector3Tmp<T>& lVec, const T& val)
	{
		if (val == static_cast<T>(0))
		{
			return lVec;
		}

		/*if constexpr (std::is_floating_point<T>())
		{
			auto result = DirectX::XMVectorMod / (lVec.ToSimdVector(), DirectX::XMVectorSet(val, val, val, 0));
			return { result.m128_f32[0], result.m128_f32[1], result.m128_f32[2] };
		}*/
		//else
		{
			return { std::fmod(lVec.x , val), std::fmod(lVec.y , val), std::fmod(lVec.z , val) };
		}
	}

	template<ValueC T>
	bool operator==(const Vector3Tmp<T>& lVec, const Vector3Tmp<T>& rVec)
	{
		return lVec.x == rVec.x && lVec.y == rVec.y && lVec.z == rVec.z;
	}


	

	

	using Vector3 = Vector3Tmp<float>;
	using Vector3D = Vector3Tmp<double>;
	using Vector3I = Vector3Tmp<int>;
};
