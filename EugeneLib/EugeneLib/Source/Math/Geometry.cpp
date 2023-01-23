#include "../../Include/Math/Geometry.h"
#include <cmath>

float GetNormalizedAngle(float angle, float valMin, float valMax)
{
	float cycle = valMax - valMin;
	float result = std::fmod((angle - valMin), cycle + valMin);
	if (result < valMin)
	{
		result += cycle;
	}
	return result;
}

double GetNormalizedAngle(double angle, double valMin, double valMax)
{
	double cycle = valMax - valMin;
	double result = std::fmod((angle - valMin), cycle + valMin);
	if (result < valMin)
	{
		result += cycle;
	}
	return result;
}

void Eugene::Get2DMatrix(Matrix4x4& out, const Vector2& size)
{
	DirectX::XMStoreFloat4x4(&out, DirectX::XMMatrixIdentity());
	out.m[0][0] = 2.0f / size.x;
	out.m[1][1] = -2.0f / size.y;
	out.m[3][0] = -1.0f;
	out.m[3][1] = 1.0f;
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
