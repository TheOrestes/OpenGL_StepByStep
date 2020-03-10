
#version 400

in vec3 localPos;

layout (location = 0) out vec4 outColor;

uniform sampler2D HDRITexture;

// this is equivalent to vec2(1/2Pi, 1/Pi)
const vec2 invATan = vec2(0.1591f, 0.3183f);

// Take XYZ co-ord on a sphere & map it into UV co-ord on the texture!
vec2 SampleSphericalMap(vec3 v)
{
	// first find out Pitch & Yaw for direction vector...
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));

	// convert pitch (-Pi, Pi) --> (-0.5, 0.5)
	// convert yaw (-Pi/2, Pi/2) --> (-0.5, 0.5)
	uv *= invATan;

	// bring it to (0,1) range for UV lookup!
	uv += 0.5f;

	// return the remapped UV :) 
	return uv;
}

void main()
{
	vec2 uv = SampleSphericalMap(normalize(localPos));
	vec4 hdriColor = texture(HDRITexture, uv);

	outColor = hdriColor;
}
