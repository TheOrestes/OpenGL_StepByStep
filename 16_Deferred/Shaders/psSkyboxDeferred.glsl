
#version 400

in vec3 vs_outTex;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec3 gAlbedo;

uniform samplerCube cubeMap;

void main()
{
	gPosition = vec3(0);
	gNormal = vec4(0);
	gAlbedo = texture(cubeMap, -vs_outTex).rgb;

	// Don't consider Cubemap for brightness pass to bloom!
	// brightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
