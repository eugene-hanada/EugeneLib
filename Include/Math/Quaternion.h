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
	/// �N�H�[�^�j�I��(float32�Œ�)
	/// </summary>
	class Quaternion
	{
	public:
		/// <summary>
		/// �f�t�H���g�R���X�g���N�^�A�e�v�f��0�ŏ����������
		/// </summary>
		constexpr Quaternion() :
			x{0.0f}, y{0.0f}, z{0.0f}, w{1.0f}
		{}

		/// <summary>
		/// �v�f�w��ł���R���X�g���N�^
		/// </summary>
		/// <param name="inX"> x </param>
		/// <param name="inY"> y </param>
		/// <param name="inZ"> z </param>
		/// <param name="inW"> w </param>
		constexpr Quaternion(float inX, float inY, float inZ, float inW):
			x{inX}, y{inY}, z{inZ}, w{inW}
		{}

		/// <summary>
		/// �R�s�[�R���X�g���N�^
		/// </summary>
		/// <param name="q"></param>
		constexpr Quaternion(const Quaternion& q):
			x{q.x}, y{q.y}, z{q.z}, w{q.w}
		{}

		/// <summary>
		/// �I�C���[�p�ŒP�ʂ͌ʓx�@�ŏ���������R���X�g���N�^
		/// </summary>
		/// <param name="rotX"> x�̊p�x </param>
		/// <param name="rotY"> y�̊p�x </param>
		/// <param name="rotZ"> z�̊p�x </param>
		Quaternion(float rotX, float rotY, float rotZ);

		/// <summary>
		/// �s�񂩂珉��������R���X�g���N�^
		/// </summary>
		/// <param name="matrix"> �s�� </param>
		Quaternion(const Matrix4x4& matrix);

		/// <summary>
		/// �������擾����
		/// </summary>
		/// <param name=""></param>
		/// <returns> ���� </returns>
		const float Magnitude(void) const;

		/// <summary>
		/// �����̓����擾����
		/// </summary>
		/// <param name=""></param>
		/// <returns> ���� </returns>
		const float SqMagnitude(void) const;

		/// <summary>
		/// ���K������
		/// </summary>
		/// <param name=""></param>
		void Normalize(void);

		/// <summary>
		/// ���K�����ꂽ�N�H�[�^�j�I�����擾����
		/// </summary>
		/// <param name=""></param>
		/// <returns> ���K�����ꂽ�N�H�[�^�j�I�� </returns>
		Quaternion Normalized(void) const;

		/// <summary>
		/// �I�C���[�p�ɕϊ������l���擾����
		/// </summary>
		/// <param name=""></param>
		/// <returns> �I�C���[�e(x,y,z�ŒP�ʂ͌ʓx�@) </returns>
		Vector3 ToEuler(void) const;

		/// <summary>
		/// �N�H�[�^�j�I�����s��ɕϊ������l���擾����
		/// </summary>
		/// <param name="out"> �o�͐�̍s�� </param>
		void ToMatrix(Matrix4x4& out) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="dir"></param>
		/// <param name="up"></param>
		void Look(const Vector3& dir, const Vector3& up);

		/// <summary>
		/// �R�s�[���Z�q
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
		/// �N�H�[�^�j�I����������
		/// </summary>
		/// <param name="q"> �E�Ӓl </param>
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
