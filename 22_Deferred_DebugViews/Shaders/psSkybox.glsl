
#version 400

in vec3 vs_outTex;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 brightColor;

uniform samplerCube cubeMap;

void main()
{
	outColor = texture(cubeMap, -vs_outTex);

	// Don't consider Cubemap for brightness pass to bloom!
	brightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
