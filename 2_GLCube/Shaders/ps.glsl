
#version 400

in vec4 color;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 brightColor;

void main()
{
	outColor = color;
}