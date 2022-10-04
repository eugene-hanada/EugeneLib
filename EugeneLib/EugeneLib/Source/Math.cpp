#include <cmath>
#include "Math.h"

namespace EugeneLib
{

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

}

