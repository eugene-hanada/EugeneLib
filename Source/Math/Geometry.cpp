#include "../../Include/Math/Geometry.h"
#include <cmath>

float Eugene::GetNormalizedAngle(float angle, float valMin, float valMax)
{
	float cycle = valMax - valMin;
	float result = std::fmod((angle - valMin), cycle + valMin);
	if (result < valMin)
	{
		result += cycle;
	}
	return result;
}

double Eugene::GetNormalizedAngle(double angle, double valMin, double valMax)
{
	double cycle = valMax - valMin;
	double result = std::fmod((angle - valMin), cycle + valMin);
	if (result < valMin)
	{
		result += cycle;
	}
	return result;
}

Eugene::Quaternion Eugene::Slearp(const Quaternion& a, const Quaternion& b, float t)
{
	auto result = DirectX::XMQuaternionSlerp(DirectX::XMVectorSet(a.x, a.y, a.z, a.w), DirectX::XMVectorSet(b.x, b.y, b.z, b.w), t);
	return { result.m128_f32[0],result.m128_f32[1],result.m128_f32[2], result.m128_f32[3] };
}

void Eugene::Get2DMatrix(Matrix4x4& out, const Vector2& size)
{
	DirectX::XMStoreFloat4x4(&out, DirectX::XMMatrixIdentity());
	out.m[0][0] = 2.0f / size.x;
	out.m[1][1] = -2.0f / size.y;
	out.m[3][0] = -1.0f;
	out.m[3][1] = 1.0f;
}

void Eugene::Get2DTranslateMatrix(Matrix4x4& out, const Vector2& pos)
{
	DirectX::XMStoreFloat4x4(&out, DirectX::XMMatrixTranslation(pos.x, pos.y, 0.0f));
}

void Eugene::Get2DTransformMatrix(Matrix4x4& out, const Vector2& pos, float rot, const Vector2& scale,const Vector2& rotPos)
{
	DirectX::XMStoreFloat4x4(&out, 
		DirectX::XMMatrixTranslation(-rotPos.x, -rotPos.y, 0.0f) *
		DirectX::XMMatrixRotationZ(rot) *
		DirectX::XMMatrixScaling(scale.x, scale.y, 1.0f) *
		DirectX::XMMatrixTranslation(pos.x, pos.y, 0.0f));
}

void Eugene::GetLookAtMatrix(Matrix4x4& out, const Vector3& eye, const Vector3& target, const Vector3& up)
{
	DirectX::XMFLOAT3 tmpEye{ eye.x,eye.y,eye.z };
	DirectX::XMFLOAT3 tmpTarget{ target.x,target.y, target.z };
	DirectX::XMFLOAT3 tmpUp{ up.x,up.y, up.z };
	DirectX::XMStoreFloat4x4(
		&out, 
		DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&tmpEye), DirectX::XMLoadFloat3(&tmpTarget), DirectX::XMLoadFloat3(&tmpUp))
	);
}

void Eugene::GetPerspectiveFovMatrix(Matrix4x4& out, float fov, float aspect, float near, float far)
{
	DirectX::XMStoreFloat4x4(&out, DirectX::XMMatrixPerspectiveFovLH(fov, aspect, near, far));
}

void Eugene::GetOrthographicMatrix(Matrix4x4& out, const Vector2& size, float near, float far)
{
	DirectX::XMStoreFloat4x4(&out, DirectX::XMMatrixOrthographicLH(size.x, size.y, near, far));
}

void Eugene::GetQuaternionToMatrix(Matrix4x4& out, const Quaternion& q)
{
	DirectX::XMFLOAT4 qf{ q.x,q.y,q.z, q.w };
	DirectX::XMStoreFloat4x4(&out, DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&qf)));
}

void Eugene::GetTransformMatrix(Matrix4x4& out, const Quaternion& q, const Vector3& pos, const Vector3& scale)
{
	//DirectX::XMFLOAT4 qf{ q.x,q.y,q.z, q.w };
	DirectX::XMStoreFloat4x4( &out, 
		DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) *
		DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(q.x,q.y,q.z, q.w)) *
		DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z)
	);
	/*DirectX::XMStoreFloat4x4(
		&out,
		DirectX::XMMatrixTranslation(-pos.x, -pos.y, -pos.z) *
		DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&qf)) *
		DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) *
		DirectX::XMMatrixTranslation(pos.x * 2.0f, pos.y * 2.0f, pos.z * 2.0f)
	);*/
}

void Eugene::Mul(Matrix4x4& out, const Matrix4x4& l, const Matrix4x4& r)
{
	DirectX::XMStoreFloat4x4(&out, DirectX::XMLoadFloat4x4(&l)* DirectX::XMLoadFloat4x4(&r));
}

void Eugene::Identity(Matrix4x4& out)
{
	DirectX::XMStoreFloat4x4(&out, DirectX::XMMatrixIdentity());
}

void Eugene::Transpose(Matrix4x4& out)
{
	DirectX::XMStoreFloat4x4(&out, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&out)));
}

void Eugene::TransposeMul(Matrix4x4& out, Matrix4x4& matrix)
{
	DirectX::XMStoreFloat4x4(&out, DirectX::XMMatrixMultiplyTranspose(DirectX::XMLoadFloat4x4(&matrix),DirectX::XMLoadFloat4x4(&matrix)));
}

void Eugene::Inverse(Matrix4x4& out)
{
	DirectX::XMVECTOR vec;
	DirectX::XMStoreFloat4x4(&out, DirectX::XMMatrixInverse(&vec, DirectX::XMLoadFloat4x4(&out)));
}

void Eugene::GetTranslateMatrix(Matrix4x4& out, const Vector3& pos)
{
	DirectX::XMStoreFloat4x4(&out, DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z));
}

void Eugene::GetRotationMatrix(Matrix4x4& out, const Quaternion& q)
{
	DirectX::XMFLOAT4 qf{ q.x,q.y,q.z, q.w };
	DirectX::XMStoreFloat4x4(&out, DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&qf)));
}

void Eugene::DeposeTransformMatrix(const Matrix4x4& out, Vector3* outTrans, Quaternion* outQ, Vector3* outScale)
{
	DirectX::XMVECTOR scale, trans, q;
	DirectX::XMMatrixDecompose(&scale, &q, &trans, DirectX::XMLoadFloat4x4(&out));
	if (outTrans)
	{
		*outTrans = { trans.m128_f32[0], trans.m128_f32[1], trans.m128_f32[2] };
	}
	if (outQ)
	{
		*outQ = { q.m128_f32[0], q.m128_f32[1], q.m128_f32[2],q.m128_f32[3] };
	}
	if (outScale)
	{
		*outScale = { scale.m128_f32[0], scale.m128_f32[1], scale.m128_f32[2] };
	}
}
