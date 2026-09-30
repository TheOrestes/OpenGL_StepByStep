
// Based on https://learnopengl.com/PBR/IBL/Diffuse-irradiance

#version 400

#define PI 3.14159265359

in vec3 localPos;

layout (location = 0) out vec4 outColor;

uniform samplerCube CubemapTexture;

void main()
{
	// final output
	vec3 irradiance = vec3(0);

	// find out hemisphere orientation
	vec3 N = normalize(localPos);
	vec3 Up = vec3(0,1,0);
	vec3 Right = cross(Up, N);
	Up = cross(N, Right);

	float sampleDelta = 0.010f;
	float NrSamples = 0.0f;

	// solidangle to spherical angle integral --> Approximation using Riemann sum!
	for(float phi = 0.0f ; phi < 2.0f * PI ; phi += sampleDelta)
	{
		for(float theta = 0.0f ; theta < 0.5f * PI ; theta += sampleDelta)
		{
			// spherical to cartesian (in tangent space)
			vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));

			// tangent space to world 
			vec3 sampleVec = tangentSample.x * Right + tangentSample.y * Up + tangentSample.z * N;

			irradiance += texture(CubemapTexture, sampleVec).rgb * cos(theta) * sin(theta);
			NrSamples++;
		}
	}

	irradiance = PI * irradiance * (1.0f / float(NrSamples));

	outColor = vec4(irradiance, 1);
}
