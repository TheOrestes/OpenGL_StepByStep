
#version 400

in vec3 vs_outPosition;
in vec4 vs_outColor;

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec3 gAlbedo;
layout (location = 3) out vec3 gEmission;
layout (location = 5) out vec3 gSkybox;
layout (location = 6) out vec3 gObjectID;

void main()
{
	gPosition.xyz = vs_outPosition;
	gNormal = vec4(0,0,0,0);
	gAlbedo = vs_outColor.rgb;
	gSkybox = vec3(0);

	// set Red channel for light mesh objects!
	gObjectID = vec3(1,0,0);

	// Find out brightness for this fragment & fill Emission buffer
	float brightness = dot(gAlbedo, vec3(0.2126f, 0.7152f, 0.0722f));
	if(brightness > 1.0f)
		gEmission = gAlbedo;
	else 
		gEmission = vec3(0.0f, 0.0f, 0.0f);
}