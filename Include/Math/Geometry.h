#pragma once
#include <algorithm>
#include "../ThirdParty/DirectXMath/DirectXMath.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Quaternion.h"

namespace Eugene
{

	// Vector2Tmpのx=0,y=0を表す定数
	template<ValueC T>
	constexpr Vector2Tmp<T> zeroVector2{ static_cast<T>(0),static_cast<T>(0) };

	// Vector2Tmpの上方向x=0,y=-1を表す定数
	template<ValueC T>
	constexpr Vector2Tmp<T> upVector2{ static_cast<T>(0),static_cast<T>(-1) };

	// Vector2Tmpの下方向x=0,y=1を表す定数
	template<ValueC T>
	constexpr Vector2Tmp<T> downVector2{ static_cast<T>(0),static_cast<T>(1) };

	// Vector2Tmpの右方向x=1,y=0を表す定数
	template<ValueC T>
	constexpr Vector2Tmp<T> rightVector2{ static_cast<T>(1),static_cast<T>(0) };

	// Vector2Tmpの上方向x=-1,y=0を表す定数
	template<ValueC T>
	constexpr Vector2Tmp<T> leftVector2{ static_cast<T>(-1),static_cast<T>(0) };

	template<ValueC T>
	constexpr Vector3Tmp<T> zeroVector3{ static_cast<T>(0),static_cast<T>(0) ,static_cast<T>(0) };

	template<ValueC T>
	constexpr Vector3Tmp<T> upVector3{ static_cast<T>(0),static_cast<T>(1) ,static_cast<T>(0) };

	template<ValueC T>
	constexpr Vector3Tmp<T> downVector3{ static_cast<T>(0),static_cast<T>(-1) ,static_cast<T>(0) };

	template<ValueC T>
	constexpr Vector3Tmp<T> forwardVector3{ static_cast<T>(0),static_cast<T>(0) ,static_cast<T>(1) };

	template<ValueC T>
	constexpr Vector3Tmp<T> backVector3{ static_cast<T>(0),static_cast<T>(0) ,static_cast<T>(-1) };

	template<ValueC T>
	constexpr Vector3Tmp<T> leftVector3{ static_cast<T>(-1),static_cast<T>(0) ,static_cast<T>(0) };

	template<ValueC T>
	constexpr Vector3Tmp<T> rightVector3{ static_cast<T>(1),static_cast<T>(0) ,static_cast<T>(0) };

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

	/// <summary>
	/// 内積(Vector2用)
	/// </summary>
	/// <param name="lVec"></param>
	/// <param name="rVec"></param>
	/// <returns></returns>
	template<ValueC T>
	constexpr T Dot(const Vector2Tmp<T>& lVec, const Vector2Tmp<T>& rVec)
	{
		return lVec.x * rVec.x + lVec.y * rVec.y;
	}


	/// <summary>
	/// 外積(Vector2用)
	/// </summary>
	/// <param name="lVec"></param>
	/// <param name="rVec"></param>
	/// <returns></returns>
	template<ValueC T>
	constexpr T Cross(const Vector2Tmp<T>& lVec, const Vector2Tmp<T>& rVec)
	{
		return lVec.x * rVec.y - lVec.y * rVec.x;
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
	constexpr Vector3Tmp<T> Deg2Rad(const Vector3Tmp<T>& deg)
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
	constexpr Vector3Tmp<T> Rad2Deg(const Vector3Tmp<T>& rad)
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



	/// <summary> Vector2とVector2の間の角度を求める </summary>
	/// <param name="v1"></param>
	/// <param name="v2"></param>
	/// <returns></returns>
	template<ValueC T>
	T GetAngle2Vector(const Vector2Tmp<T>& v1, const Vector2Tmp<T>& v2)
	{
		auto tmpv1 = v1.Normalized();
		auto tmpv2 = v2.Normalized();
		auto dot = Dot(tmpv1, tmpv2);
		auto cross = Cross(tmpv1, tmpv2);
		auto angle = atan2(cross, dot);
		if (angle < 0)
		{
			angle += pi<float> *2.0f;
		}
		return angle;
	}



	/// <summary> aとbをtで線形補完する関数 </summary>
	/// <param name="a">  </param>
	/// <param name="b"></param>
	/// <param name="t">　1.0f から0.0fの値　</param>
	/// <returns></returns>
	template<ValueC T>
	constexpr T Learp(const T& a, const T& b, const T& t)
	{
		return a + ((b - a) * t);
	}

	/// <summary> ベクトルaとベクトルbをtで線形補完する関数 </summary>
	/// <param name="a">  </param>
	/// <param name="b"></param>
	/// <param name="t"> 1.0f から0.0fの値 </param>
	/// <returns></returns>
	template<ValueC T>
	constexpr Vector2Tmp<T> Learp(const Vector2Tmp<T>& a, const Vector2Tmp<T>& b, const T& t)
	{
		return a + ((b - a) * t);
	}

	template<ValueC T>
	constexpr Vector3Tmp<T> Learp(const Vector3Tmp<T>& a, const Vector3Tmp<T>& b, const T& t)
	{
		if constexpr (std::is_floating_point<T>() && sizeof(T) == 4)
		{
			auto result = DirectX::XMVectorLerp(DirectX::XMVectorSet(a.x, a.y, a.z, 0.0f), DirectX::XMVectorSet(b.x, b.y, b.z, 0.0f), t);
			return { result.m128_f32[0],result.m128_f32[1], result.m128_f32[2] };
		}
		else
		{
			return a + ((b - a) * t);
		}
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="a"></param>
	/// <param name="b"></param>
	/// <param name="t"></param>
	/// <returns></returns>
	Quaternion Slearp(const Quaternion& a, const Quaternion& b, float t);

	/// <summary> Vector2TmpをClampする </summary>
	/// <param name="val"> Clampしたい値 </param>
	/// <param name="min"> 最小値 </param>
	/// <param name="max"> 最大値 </param>
	/// <returns> Clamp後の値 </returns>
	template<ValueC T>
	constexpr Vector2Tmp<T> Clamp(const Vector2Tmp<T>& val, const Vector2Tmp<T>& min, const Vector2Tmp<T>& max)
	{
		return { std::clamp(val.x,min.x,max.x),std::clamp(val.y,min.y,max.y) };
	}


	using Matrix4x4 = DirectX::XMFLOAT4X4;

	/// <summary>
	/// 2Dのレンダーターゲット用のスクリーン座標をクリップ座標変換する行列
	/// </summary>
	/// <param name="out"></param>
	/// <param name="size"> レンダーターゲットのサイズ </param>
	void Get2DMatrix(Matrix4x4& out, const Vector2& size);

	/// <summary>
	/// 2Dで平行移動する行列
	/// </summary>
	/// <param name="out"></param>
	/// <param name="pos"> 移動座標 </param>
	void Get2DTranslateMatrix(Matrix4x4& out, const Vector2& pos);

	/// <summary>
	/// 2Dの変換行列(移動、拡縮、回転)
	/// </summary>
	/// <param name="out"></param>
	/// <param name="pos"> 移動座標 </param>
	/// <param name="rot"> 回転角度(ラジアン) </param>
	/// <param name="scale"> 拡縮(スケール) </param>
	/// <param name="rotPos"> 回転基準座標 </param>
	void Get2DTransformMatrix(Matrix4x4& out, const Vector2& pos, float rot = 0.0f, const Vector2& scale = { 0.0f,0.0f }, const Vector2& rotPos = zeroVector2<float>);

	/// <summary>
	/// 特定方向を向く行列
	/// </summary>
	/// <param name="out"></param>
	/// <param name="eye"> 視点座標 </param>
	/// <param name="target"> 向きたい座標 </param>
	/// <param name="up"> 上方向 </param>
	void GetLookAtMatrix(Matrix4x4& out, const Vector3& eye, const Vector3& target, const Vector3& up);

	/// <summary>
	/// パースペクティブ行列
	/// </summary>
	/// <param name="out"></param>
	/// <param name="fov"> 視野角 </param>
	/// <param name="aspect"> アスペクト比 </param>
	/// <param name="near"> ニア </param>
	/// <param name="far"> ファー </param>
	void GetPerspectiveFovMatrix(Matrix4x4& out, float fov, float aspect, float near = 0.1f, float far = 1000.0f);

	/// <summary>
	/// 投影行列
	/// </summary>
	/// <param name="out"></param>
	/// <param name="size"> サイズ </param>
	/// <param name="near"> ニア </param>
	/// <param name="far"> ファー </param>
	void GetOrthographicMatrix(Matrix4x4& out, const Vector2& size, float near = 0.1f, float far = 1000.0f);

	/// <summary>
	/// クォータニオンから回転行列を取得する
	/// </summary>
	/// <param name="out"> 出力する行列 </param>
	/// <param name="q"> クォータニオン </param>
	void GetQuaternionToMatrix(Matrix4x4& out, const Quaternion& q);

	/// <summary>
	/// 回転、座標、拡縮から行列を取得する
	/// </summary>
	/// <param name="out"></param>
	/// <param name="q"> 回転(クォータニオン) </param>
	/// <param name="pos"> 座標 </param>
	/// <param name="scale"> スケール </param>
	void GetTransformMatrix(Matrix4x4& out, const Quaternion& q, const Vector3& pos, const Vector3& scale);

	/// <summary>
	/// 行列の積
	/// </summary>
	/// <param name="out"></param>
	/// <param name="l"></param>
	/// <param name="r"></param>
	void Mul(Matrix4x4& out, const Matrix4x4& l, const Matrix4x4& r);

	/// <summary>
	/// 単位行列
	/// </summary>
	/// <param name="out"></param>
	void Identity(Matrix4x4& out);


	/// <summary>
	/// 転置行列にする
	/// </summary>
	/// <param name="out"></param>
	void Transpose(Matrix4x4& out);

	/// <summary>
	/// 転置行列をかける
	/// </summary>
	/// <param name="out"></param>
	/// <param name="matrix"></param>
	void TransposeMul(Matrix4x4& out, Matrix4x4& matrix);

	/// <summary>
	/// 逆行列にする
	/// </summary>
	/// <param name="out"></param>
	void Inverse(Matrix4x4& out);

	/// <summary>
	/// 移動する行列を取得する
	/// </summary>
	/// <param name="out"></param>
	/// <param name="pos"></param>
	void GetTranslateMatrix(Matrix4x4& out, const Vector3& pos);

	/// <summary>
	/// 回転する行列を取得する
	/// </summary>
	/// <param name="out"></param>
	/// <param name="q"></param>
	void GetRotationMatrix(Matrix4x4& out, const Quaternion& q);

	/// <summary>
	/// 行列から移動、回転、スケール取得する
	/// </summary>
	/// <param name="out"></param>
	/// <param name="outTrans"></param>
	/// <param name="outQ"></param>
	/// <param name="outScale"></param>
	void DeposeTransformMatrix(const Matrix4x4& out, Vector3* outTrans, Quaternion* outQ, Vector3* outScale);

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