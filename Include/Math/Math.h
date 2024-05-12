#pragma once
#include "../ThirdParty/glm/glm/glm.hpp"
#include "../ThirdParty/glm/glm/ext.hpp"
#include "MathConcepts.h"

namespace Eugene
{
	using namespace glm;

	/// <summary>
	/// Vector2Tmp��x=0,y=0��\���萔
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<ValueC T>
	constexpr vec<2, T> zeroVector2{ static_cast<T>(0),static_cast<T>(0) };

	/// <summary>
	/// Vector2Tmp�̏����x=0,y=-1��\���萔
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<ValueC T>
	constexpr vec<2, T> upVector2{ static_cast<T>(0),static_cast<T>(-1) };

	/// <summary>
	/// Vector2Tmp�̉�����x=0,y=1��\���萔
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<ValueC T>
	constexpr vec<2, T> downVector2{ static_cast<T>(0),static_cast<T>(1) };

	/// <summary>
	/// Vector2Tmp�̉E����x=1,y=0��\���萔
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<ValueC T>
	constexpr vec<2, T> rightVector2{ static_cast<T>(1),static_cast<T>(0) };

	/// <summary>
	/// Vector2Tmp�̏����x=-1,y=0��\���萔
	/// </summary>
	/// <typeparam name="T"></typeparam>
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
	/// ��悷��
	/// </summary>
	/// <param name="val"> ��悵�����l </param>
	/// <returns> ��悵���l </returns>
	template<ValueC T>
	constexpr T Square(T val)
	{
		return val * val;
	}

	// �~����
	template<ValueC T>
	constexpr T pi = static_cast<T>(3.1415926535897932384626433832795);

	// �~����*2
	template<ValueC T>
	constexpr T pi2 = pi<T> *static_cast<T>(2.0);

	/// <summary>
	/// �x���@���ʓx�@�ɂ���
	/// </summary>
	/// <param name="deg"> �ϊ��������x���@�̒l </param>
	/// <returns>  </returns>
	template<ValueC T>
	constexpr T Deg2Rad(const T& deg)
	{
		return deg * (pi<T> / static_cast<T>(180));
	}

	template<ValueC T>
	constexpr glm::vec<3, T> Deg2Rad(const glm::vec<3, T>& deg)
	{
		return { Deg2Rad(deg.x),Deg2Rad(deg.y) ,Deg2Rad(deg.z) };
	}

	/// <summary>
	/// �ʓx�@��x���@�ɂ���
	/// </summary>
	/// <param name="rad"></param>
	/// <returns></returns>
	template<ValueC T>
	constexpr T Rad2Deg(const T& rad)
	{
		return rad * (static_cast<T>(180) / pi<T>);
	}

	template<ValueC T>
	constexpr glm::vec<3, T> Rad2Deg(const glm::vec<3, T>& rad)
	{
		return { Rad2Deg(rad.x),Rad2Deg(rad.y) ,Rad2Deg(rad.z) };
	}

	/// <summary>
	/// �p�x���w��͈͂ɐ��K������
	/// </summary>
	/// <param name="angle"> �ϊ�����l </param>
	/// <param name="valMin"> �ŏ��l(�f�t�H���g=0) </param>
	/// <param name="valMax"> �ő�l(�f�t�H���g=pi2) </param>
	/// <returns></returns>
	float GetNormalizedAngle(float angle, float valMin = 0.0f, float valMax = pi2<float>);

	/// <summary>
	/// �p�x�̐��K��(��̂��double��)
	/// </summary>
	/// <param name="angle"></param>
	/// <param name="valMin"></param>
	/// <param name="valMax"></param>
	/// <returns></returns>
	double GetNormalizedAngle(double angle, double valMin = 0.0f, double valMax = pi2<double>);
}

