
#version 400

in vec3 vs_outTex;

layout (location = 0) out vec4 outColor;

uniform samplerCube cubeMap;

void main()
{
	outColor = texture(cubeMap, -vs_outTex);
}
