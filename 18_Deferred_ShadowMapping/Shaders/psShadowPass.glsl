
#version 400

layout (location = 0) out float shadowDepth;

void main()
{
	// Not needed since OpenGL does it anyways!
	//shadowDepth = gl_FragCoord.z;
}