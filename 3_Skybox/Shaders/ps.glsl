
#version 400

in vec4 color;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 brightColor;

void main()
{
	outColor = color;

	// check whether fragment color is more than the threshold brightness value
	// we calculate first grayscale equivalent...
	float brightness = dot(outColor.rgb, vec3(0.2126f, 0.7152f, 0.0722f));
	if(brightness > 1.0f)
		brightColor = vec4(outColor.rgb, 1.0f);
}