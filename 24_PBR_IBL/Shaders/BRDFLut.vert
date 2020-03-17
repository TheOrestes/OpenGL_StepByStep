
#version 400

layout(location=0) in vec3 in_Position;
layout(location=1) in vec2 in_Texcoord;

out vec2 out_Texcoord;

// uniforms
uniform mat4 matView;
uniform mat4 matProj;

void main()
{
	out_Texcoord = in_Texcoord;
	gl_Position = vec4(in_Position, 1.0f);
}