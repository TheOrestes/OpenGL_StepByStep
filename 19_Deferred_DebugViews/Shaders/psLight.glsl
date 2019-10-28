
#version 400

in vec4 color;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 brightColor;

void main()
{
	outColor = color;

	// Find out brightness for this fragment
	float brightness = dot(color.rgb, vec3(0.2126f, 0.7152f, 0.0722f));
	if(brightness > 1.0f)
		brightColor = vec4(color.rgb, 1.0f);
	else 
		brightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}