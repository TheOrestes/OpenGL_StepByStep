
#version 400

layout(location=0) in vec3 in_Position;

out vec3 localPos;

// uniforms
uniform mat4 matView;
uniform mat4 matProj;

void main()
{
	localPos = in_Position;

	mat4 wvp = matProj * matView;
	gl_Position = wvp * vec4(in_Position, 1.0f);
}