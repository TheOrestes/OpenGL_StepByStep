
#version 400

in vec3 vs_outTex;

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec3 gAlbedo;
layout (location = 3) out vec3 gEmission;
layout (location = 4) out vec3 gMask;
layout (location = 5) out vec3 gSkybox;
layout (location = 6) out vec3 gObjectID;

uniform samplerCube cubeMap;

void main()
{
	gPosition = vec4(0);
	gNormal = vec4(0);
	gAlbedo = vec3(0);
	gEmission = vec3(0);
	gMask = vec3(0);
	gSkybox = texture(cubeMap, -vs_outTex).rgb;

	// set Green channel for Skybox!
	gObjectID = vec3(0, 1, 0);
	
	// Find out brightness for this fragment & fill Emission buffer
	//float brightness = dot(gAlbedo, vec3(0.2126f, 0.7152f, 0.0722f));
	//if(brightness > 50.0f)
	//	gEmission = gAlbedo;
	//else 
	//	gEmission = vec3(0.0f, 0.0f, 0.0f);
}
