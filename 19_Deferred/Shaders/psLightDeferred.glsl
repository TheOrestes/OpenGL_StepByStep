
#version 400

in vec3 vs_outPosition;
in vec4 vs_outColor;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec3 gAlbedo;

void main()
{
	gPosition = vs_outPosition;
	gNormal = vec4(0,0,0,0);
	gAlbedo = vs_outColor.rgb;

	// Find out brightness for this fragment
	//float brightness = dot(color.rgb, vec3(0.2126f, 0.7152f, 0.0722f));
	//if(brightness > 1.0f)
	//	brightColor = vec4(color.rgb, 1.0f);
	//else 
	//	brightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}