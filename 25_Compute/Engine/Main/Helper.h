#pragma once

#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/norm.hpp"
#include <time.h>

const int START_WINDOW_WIDTH = 1280;
const int START_WINDOW_HEIGHT = 800;

const int SHADOWMAP_SIZE = 1024;

template<typename T> void SAFE_DELETE(T*& a)
{
	delete a;
	a = nullptr;
}

namespace Helper
{
	inline std::string GetCurrentDateTime()
	{
		time_t now = time(0);
		struct tm tstruct;
		char buf[80];

		tstruct = *localtime(&now);
		strftime(buf, sizeof(buf), "%d_%B_%Y_%H_%M_%S", &tstruct);

		return buf;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline glm::vec3 LerpVector(const glm::vec3& vec1, const glm::vec3& vec2, float t)
	{
		return (1.0f - t) * vec1 + t * vec2;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline float GetRandom01()
	{
		return ((float)rand() / (RAND_MAX + 1));
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline float GetRandomRange(float min, float max)
	{
		return min + (max - min) * GetRandom01();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline glm::vec3 GetRandomInUnitDisk()
	{
		glm::vec3 p;
		do
		{
			p = 2.0f * glm::vec3(GetRandom01(), GetRandom01(), 0.0f) - glm::vec3(1, 1, 0);
		} while (glm::dot(p, p) >= 1.0f);

		return p;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline glm::vec3 RandomInUnitSphere()
	{
		glm::vec3 P;

		do
		{
			P = 2.0f * glm::vec3(GetRandom01(), GetRandom01(), GetRandom01()) - glm::vec3(1, 1, 1);
		} while (glm::length2(P) >= 1.0f);

		return P;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline float RangeRemap(float value, float low1, float high1, float low2, float high2)
	{
		return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
	}
}
