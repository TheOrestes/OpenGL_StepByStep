
#version 400

in vec3 localPos;

layout (location = 0) out vec4 outColor;

uniform sampler2D HDRITexture;

const vec2 invATan = vec2(0.1591f, 0.3183f);

vec2 SampleSphericalMap(vec3 v)
{
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
	uv *= invATan;
	uv += 0.5f;

	return uv;
}

void main()
{
	vec2 uv = SampleSphericalMap(normalize(localPos));
	vec4 hdriColor = texture(HDRITexture, uv);

	outColor = hdriColor;
}
