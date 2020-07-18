
#version 400

layout (location = 0) out vec4 gPosition;
layout (location = 3) out vec3 gEmission;

in vec3 vs_outPosition;
in float vs_outLiftTime;
in vec4 vs_outVelocity;
in vec4 vs_outAcceleration;
in vec4 vs_outColor;

void main()
{
	// write to Position buffer
	gPosition = vec4(vs_outPosition, 1.0f);

	// write to Albedo buffer
	gEmission = vs_outColor.xyz;
}