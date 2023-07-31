#pragma once
#include "Vector3.h"
#include <format>

namespace DirectX
{
	struct XMFLOAT4X4;
}

namespace Eugene
{
	using Matrix4x4 = DirectX::XMFLOAT4X4;

	/// <summary>
	/// クォータニオン(float32固定)
	/// </summary>
	class Quaternion
	{
	public:
		/// <summary>
		/// デフォルトコンストラクタ、各要素は0で初期化される
		/// </summary>
		constexpr Quaternion() :
			x{0.0f}, y{0.0f}, z{0.0f}, w{1.0f}
		{}

		/// <summary>
		/// 要素指定できるコンストラクタ
		/// </summary>
		/// <param name="inX"> x </param>
		/// <param name="inY"> y </param>
		/// <param name="inZ"> z </param>
		/// <param name="inW"> w </param>
		constexpr Quaternion(float inX, float inY, float inZ, float inW):
			x{inX}, y{inY}, z{inZ}, w{inW}
		{}

		/// <summary>
		/// コピーコンストラクタ
		/// </summary>
		/// <param name="q"></param>
		constexpr Quaternion(const Quaternion& q):
			x{q.x}, y{q.y}, z{q.z}, w{q.w}
		{}

		/// <summary>
		/// オイラー角で単位は弧度法で初期化するコンストラクタ
		/// </summary>
		/// <param name="rotX"> xの角度 </param>
		/// <param name="rotY"> yの角度 </param>
		/// <param name="rotZ"> zの角度 </param>
		Quaternion(float rotX, float rotY, float rotZ);

		/// <summary>
		/// 行列から初期化するコンストラクタ
		/// </summary>
		/// <param name="matrix"> 行列 </param>
		Quaternion(const Matrix4x4& matrix);

		/// <summary>
		/// 長さを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns> 長さ </returns>
		const float Magnitude(void) const;

		/// <summary>
		/// 長さの二乗を取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns> 長さ </returns>
		const float SqMagnitude(void) const;

		/// <summary>
		/// 正規化する
		/// </summary>
		/// <param name=""></param>
		void Normalize(void);

		/// <summary>
		/// 正規化されたクォータニオンを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns> 正規化されたクォータニオン </returns>
		Quaternion Normalized(void) const;

		/// <summary>
		/// オイラー角に変換した値を取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns> オイラー各(x,y,zで単位は弧度法) </returns>
		Vector3 ToEuler(void) const;

		/// <summary>
		/// クォータニオンを行列に変換した値を取得する
		/// </summary>
		/// <param name="out"> 出力先の行列 </param>
		void ToMatrix(Matrix4x4& out) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="dir"></param>
		/// <param name="up"></param>
		void Look(const Vector3& dir, const Vector3& up);

		/// <summary>
		/// コピー演算子
		/// </summary>
		/// <param name="q"></param>
		/// <returns></returns>
		constexpr Quaternion& operator=(const Quaternion& q)
		{
			x = q.x;
			y = q.y;
			z = q.z;
			w = q.w;
			return *this;
		}

		/// <summary>
		/// クォータニオンをかける
		/// </summary>
		/// <param name="q"> 右辺値 </param>
		/// <returns></returns>
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

template<>
class std::formatter<Eugene::Quaternion> :
	public std::formatter<const char*>
{
public:
	template<class Out>
	constexpr auto parse(std::basic_format_parse_context<Out>& ctx)
	{
		auto it = ctx.begin();
		if (*it != '}')
		{
			if (*it == 'd')
			{
				isEuler_ = true;
			}
			++it;
		}
		return it;
	}

	template<class Out>
	constexpr auto format(const Eugene::Quaternion& q, std::basic_format_context<Out, char>& ctx)
	{
		return isEuler_ ? std::format_to(ctx.out(), "{}", q.ToEuler()) : std::format_to(ctx.out(), "x={0:f}y={1:f}z={2:f}w={3:f}", q.x, q.y, q.z, q.w);
	}
private:
	bool isEuler_{false};
};
