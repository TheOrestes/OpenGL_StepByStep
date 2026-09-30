#pragma once

#include "glm/glm.hpp"

const int START_WINDOW_WIDTH = 1280;
const int START_WINDOW_HEIGHT = 800;

const int SHADOWMAP_SIZE = 1024;

template<typename T> void SAFE_DELETE(T*& a)
{
	delete a;
	a = nullptr;
}

// Blackbody color temperature (Kelvin) -> linear RGB, Tanner Helland's fit to Planck's law
// (the standard real-time approximation, ~1000K-40000K). ~6600K is neutral white; below that
// warms toward orange/red (candlelight ~1900K, sunset ~2000-3000K), above cools toward blue
// (overcast sky ~7000-10000K). Used to derive a light's color from a single physical quantity
// instead of an artist-picked RGB value.
inline glm::vec3 KelvinToRGB(float kelvin)
{
	float k = glm::clamp(kelvin, 1000.0f, 40000.0f) / 100.0f;

	float r = (k <= 66.0f) ? 255.0f
		: 329.698727446f * powf(k - 60.0f, -0.1332047592f);

	float g = (k <= 66.0f) ? (99.4708025861f * logf(k) - 161.1195681661f)
		: 288.1221695283f * powf(k - 60.0f, -0.0755148492f);

	float b = (k >= 66.0f) ? 255.0f
		: (k <= 19.0f) ? 0.0f
		: 138.5177312231f * logf(k - 10.0f) - 305.0447927307f;

	return glm::clamp(glm::vec3(r, g, b), 0.0f, 255.0f) / 255.0f;
}
