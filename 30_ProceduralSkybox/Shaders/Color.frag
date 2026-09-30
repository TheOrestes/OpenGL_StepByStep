
#version 400

in vec4 out_Color;

layout (location = 0) out vec4 outColor;

void main()
{
	outColor = out_Color;
}