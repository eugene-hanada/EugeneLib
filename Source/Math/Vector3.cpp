#include "../../Include/Math/Vector3.h"
#include "../../Include/ThirdParty/DirectXMath/DirectXMath.h"
namespace Eugene
{

	template<ValueC T>
	T Vector3Tmp<T>::Magnitude(void) const
	{
		if constexpr (std::is_floating_point<T>() && sizeof(T) == 4)
		{
			return DirectX::XMVector3Length(DirectX::XMVectorSet(x, y, z, 0)).m128_f32[0];
		}
		else
		{
			return static_cast<T>(std::sqrt(x * x + y * y + z * z));
		}

	}

	template<ValueC T>
	T Vector3Tmp<T>::SqMagnitude(void) const
	{
		if constexpr (std::is_floating_point<T>() && sizeof(T) == 4)
		{
			return DirectX::XMVector3LengthSq(DirectX::XMVectorSet(x, y, z, 0)).m128_f32[0];
		}
		else
		{
			return x * x + y * y + z * z;
		}
	}

	template<ValueC T>
	void Vector3Tmp<T>::Normalize(void)
	{
		T mag = Magnitude();
		if (mag == static_cast<T>(0))
		{
			return;
		}
		x /= mag;
		y /= mag;
		z /= mag;
	}

	template<ValueC T>
	Vector3Tmp<T> Vector3Tmp<T>::Normalized(void) const
	{
		T mag = Magnitude();
		return { static_cast<T>(x / mag), static_cast<T>(y / mag) , static_cast<T>( z / mag) };
	}

	template<ValueC T>
	Vector3Tmp<T> Vector3Tmp<T>::Refrect(const Vector3Tmp<T>& n)
	{
		return (*this) - (n * static_cast<T>(static_cast<T>(2) * (x * n.x + y * n.y + z * n.z)));
	}

	template<ValueC T>
	Vector3Tmp<T> Vector3Tmp<T>::operator-() const
	{
		return { static_cast<T>(-x), static_cast<T>(-y) , static_cast<T>(-z)};
	}

	template<ValueC T>
	Vector3Tmp<T> Vector3Tmp<T>::operator+() const
	{
		return { static_cast<T>(+x),static_cast<T>(+y), static_cast<T>(+z)};
	}

	template<ValueC T>
	Vector3Tmp<T>& Vector3Tmp<T>::operator+=(const Vector3Tmp<T>& vec)
	{
		x += vec.x;
		y += vec.y;
		z += vec.z;
		return *this;
	}

	template<ValueC T>
	Vector3Tmp<T>& Vector3Tmp<T>::operator+=(const T& val)
	{
		x += val;
		y += val;
		z += val;
		return *this;
	}

	template<ValueC T>
	Vector3Tmp<T>& Vector3Tmp<T>::operator-=(const Vector3Tmp<T>& vec)
	{
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;
		return *this;
	}

	template<ValueC T>
	Vector3Tmp<T>& Vector3Tmp<T>::operator-=(const T& val)
	{
		x -= val;
		y -= val;
		z -= val;
		return *this;
	}

	template<ValueC T>
	Vector3Tmp<T>& Vector3Tmp<T>::operator*=(const Vector3Tmp<T>& vec)
	{
		x *= vec.x;
		y *= vec.y;
		z *= vec.z;
		return *this;
	}

	template<ValueC T>
	inline Vector3Tmp<T>& Vector3Tmp<T>::operator*=(const T& val)
	{
		x *= val;
		y *= val;
		z *= val;
		return *this;
	}

	template<ValueC T>
	Vector3Tmp<T>& Vector3Tmp<T>::operator/=(const Vector3Tmp<T>& vec)
	{
		if (vec.x == static_cast<T>(0) || vec.y == static_cast<T>(0) || vec.z == static_cast<T>(0))
		{
			return *this;
		}
		x /= vec.x;
		y /= vec.y;
		z /= vec.z;
		return *this;
	}

	template<ValueC T>
	Vector3Tmp<T>& Vector3Tmp<T>::operator/=(const T& val)
	{
		if (val == static_cast<T>(0))
		{
			return *this;
		}
		x /= val;
		y /= val;
		z /= val;
		return *this;
	}

	template<ValueC T>
	Vector3Tmp<T>& Vector3Tmp<T>::operator%=(const Vector3Tmp<T>& vec)
	{
		if (vec.x == static_cast<T>(0) || vec.y == static_cast<T>(0) || vec.z == static_cast<T>(0))
		{
			return *this;
		}
		x = static_cast<T>(std::fmod(x, vec.x));
		y = static_cast<T>(std::fmod(y, vec.y));
		z = static_cast<T>(std::fmod(z, vec.z));
		return *this;
	}

	template<ValueC T>
	Vector3Tmp<T>& Vector3Tmp<T>::operator%=(const T& val)
	{
		if (val == static_cast<T>(0))
		{
			return *this;
		}
		x = static_cast<T>(std::fmod(x, val));
		y = static_cast<T>(std::fmod(y, val));
		z = static_cast<T>(std::fmod(z, val));
		return *this;
	}

	// 明示的インスタンス
	template class Vector3Tmp<std::uint16_t>;
	template class Vector3Tmp<float>;
	template class Vector3Tmp<double>;
	template class Vector3Tmp<std::int32_t>;
	template class Vector3Tmp<std::int64_t>;

}