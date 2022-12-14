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

void EugeneLib::Get2DMatrix(Matrix4x4& out, const Vector2& size)
{
	DirectX::XMStoreFloat4x4(&out, DirectX::XMMatrixIdentity());
	out.m[0][0] = 2.0f / size.x;
	out.m[1][1] = -2.0f / size.y;
	out.m[3][0] = -1.0f;
	out.m[3][1] = 1.0f;
}
