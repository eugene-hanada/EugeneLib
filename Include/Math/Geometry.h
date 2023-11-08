#pragma once
#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <algorithm>
#include "../../ThirdParty/glm/glm/ext.hpp"
#include "../../ThirdParty/glm/glm/glm.hpp"
#include "MathConcepts.h"

#include "Random.h"


namespace Eugene
{
	using namespace glm;

	// Vector2Tmpのx=0,y=0を表す定数
	template<ValueC T>
	constexpr vec<2,T> zeroVector2{ static_cast<T>(0),static_cast<T>(0) };

	// Vector2Tmpの上方向x=0,y=-1を表す定数
	template<ValueC T>
	constexpr vec<2, T> upVector2{ static_cast<T>(0),static_cast<T>(-1) };

	// Vector2Tmpの下方向x=0,y=1を表す定数
	template<ValueC T>
	constexpr vec<2, T> downVector2{ static_cast<T>(0),static_cast<T>(1) };

	// Vector2Tmpの右方向x=1,y=0を表す定数
	template<ValueC T>
	constexpr vec<2, T> rightVector2{ static_cast<T>(1),static_cast<T>(0) };

	// Vector2Tmpの上方向x=-1,y=0を表す定数
	template<ValueC T>
	constexpr vec<2, T> leftVector2{ static_cast<T>(-1),static_cast<T>(0) };

	template<ValueC T>
	constexpr vec<3, T> zeroVector3{ static_cast<T>(0),static_cast<T>(0) ,static_cast<T>(0) };

	template<ValueC T>
	constexpr vec<3, T> upVector3{ static_cast<T>(0),static_cast<T>(1) ,static_cast<T>(0) };

	template<ValueC T>
	constexpr vec<3, T> downVector3{ static_cast<T>(0),static_cast<T>(-1) ,static_cast<T>(0) };

	template<ValueC T>
	constexpr vec<3, T> forwardVector3{ static_cast<T>(0),static_cast<T>(0) ,static_cast<T>(1) };

	template<ValueC T>
	constexpr vec<3, T> backVector3{ static_cast<T>(0),static_cast<T>(0) ,static_cast<T>(-1) };

	template<ValueC T>
	constexpr vec<3, T> leftVector3{ static_cast<T>(-1),static_cast<T>(0) ,static_cast<T>(0) };

	template<ValueC T>
	constexpr vec<3, T> rightVector3{ static_cast<T>(1),static_cast<T>(0) ,static_cast<T>(0) };

	/// <summary>
	/// 二乗する
	/// </summary>
	/// <param name="val"> 二乗したい値 </param>
	/// <returns> 二乗した値 </returns>
	template<ValueC T>
	constexpr T Square(T val)
	{
		return val * val;
	}

	// 円周率
	template<ValueC T>
	constexpr T pi = static_cast<T>(3.1415926535897932384626433832795);

	// 円周率*2
	template<ValueC T>
	constexpr T pi2 = pi<T> *static_cast<T>(2.0);

	/// <summary>
	/// 度数法を弧度法にする
	/// </summary>
	/// <param name="deg"> 変換したい度数法の値 </param>
	/// <returns>  </returns>
	template<ValueC T>
	constexpr T Deg2Rad(const T& deg)
	{
		return deg * (pi<T> / static_cast<T>(180));
	}

	template<ValueC T>
	constexpr glm::vec<3,T> Deg2Rad(const glm::vec<3,T>& deg)
	{
		return { Deg2Rad(deg.x),Deg2Rad(deg.y) ,Deg2Rad(deg.z) };
	}

	/// <summary>
	/// 弧度法を度数法にする
	/// </summary>
	/// <param name="rad"></param>
	/// <returns></returns>
	template<ValueC T>
	constexpr T Rad2Deg(const T& rad)
	{
		return rad * (static_cast<T>(180) / pi<T>);
	}

	template<ValueC T>
	constexpr glm::vec<3,T> Rad2Deg(const glm::vec<3,T>& rad)
	{
		return { Rad2Deg(rad.x),Rad2Deg(rad.y) ,Rad2Deg(rad.z) };
	}

	/// <summary>
	/// 角度を指定範囲に正規化する
	/// </summary>
	/// <param name="angle"> 変換する値 </param>
	/// <param name="valMin"> 最小値(デフォルト=0) </param>
	/// <param name="valMax"> 最大値(デフォルト=pi2) </param>
	/// <returns></returns>
	float GetNormalizedAngle(float angle, float valMin = 0.0f, float valMax = pi2<float>);

	/// <summary>
	/// 角度の正規化(上のやつのdouble版)
	/// </summary>
	/// <param name="angle"></param>
	/// <param name="valMin"></param>
	/// <param name="valMax"></param>
	/// <returns></returns>
	double GetNormalizedAngle(double angle, double valMin = 0.0f, double valMax = pi2<double>);

	/// <summary>
	/// アライメントされた大きさを返す
	/// </summary>
	/// <param name="size"> サイズ </param>
	/// <param name="alignment"> アライメントサイズ </param>
	/// <returns> アライメントされたサイズ </returns>
	constexpr size_t AlignmentedSize(const size_t& size, const size_t& alignment)
	{
		return size + alignment - size % alignment;
	}

	/// <summary>
	/// 上記のテンプレート版(定数時に使える)
	/// </summary>
	/// <typeparam name="Size"></typeparam>
	/// <typeparam name="Alignment"></typeparam>
	/// <returns></returns>
	template<size_t Size, size_t Alignment>
	constexpr size_t AlignmentedSize()
	{
		return Size + Alignment - Size % Alignment;
	}
};