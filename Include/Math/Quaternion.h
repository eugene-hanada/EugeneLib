#pragma once
#include "Vector3.h"

namespace DirectX
{
	struct XMFLOAT4X4;
}

namespace Eugene
{
	using Matrix4x4 = DirectX::XMFLOAT4X4;
	class Quaternion
	{
	public:
		constexpr Quaternion() :
			x{0.0f}, y{0.0f}, z{0.0f}, w{1.0f}
		{}

		constexpr Quaternion(float inX, float inY, float inZ, float inW):
			x{inX}, y{inY}, z{inZ}, w{inW}
		{}

		constexpr Quaternion(const Quaternion& q):
			x{q.x}, y{q.y}, z{q.z}, w{q.w}
		{}

		Quaternion(float rotX, float rotY, float rotZ);

		Quaternion(const Matrix4x4& matrix);

		const float Magnitude(void) const;
		const float SqMagnitude(void) const;
		void Normalize(void);
		Quaternion Normalized(void) const;
		Vector3 ToEuler(void) const;

		constexpr Quaternion& operator=(const Quaternion& q)
		{
			x = q.x;
			y = q.y;
			z = q.z;
			w = q.w;
			return *this;
		}

		Quaternion& operator*=(const Quaternion& q);

		float x;
		float y;
		float z;
		float w;
	private:
	};

	Quaternion operator*(const Quaternion& qL, const Quaternion& qR);
	Vector3 operator*(const Quaternion& q, const Vector3& vec);
}
