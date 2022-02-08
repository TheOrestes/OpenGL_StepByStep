
#version 400

////////////////////////////////////////////////////////////////////////////////////////////////////
layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 brightColor;

in vec2 vs_outTexcoord;

////////////////////////////////////////////////////////////////////////////////////////////////////
uniform sampler2D positionBuffer;
uniform sampler2D normalBuffer;
uniform sampler2D albedoBuffer;
uniform sampler2D emissiveBuffer;
uniform sampler2D shadowDepthBuffer;

uniform samplerCube	texture_skybox;

// Camera related
uniform vec3 cameraPosition;
uniform mat4 cameraViewMat;
uniform mat4 cameraProjMat;

// light/shadow related
uniform mat4 matWorldToLigthView;
uniform mat4 matLightViewToProjection;

//---------------------------------------------------------------------------------------
// Point Lights
//---------------------------------------------------------------------------------------
#define MAX_POINT_LIGHTS 8

uniform int			numPointLights;		// number of point lights in the scene
struct PointLight
{
	float radius;
	float intensity;
	vec3 position;
	vec4 color;
};

uniform PointLight pointLights[MAX_POINT_LIGHTS];

//---------------------------------------------------------------------------------------
// Directional Lights
//---------------------------------------------------------------------------------------
#define MAX_DIRECTIONAL_LIGHTS 8

uniform int			numDirectionalLights;		// number of point lights in the scene
struct DirectionalLight
{
	vec3 direction;
	vec4 color;
	float intensity;
};

uniform DirectionalLight dirLights[MAX_POINT_LIGHTS];

////////////////////////////////////////////////////////////////////////////////////////////////////
float readShadowMap(vec3 Position, vec3 Normal, vec3 viewDir)
{
	vec4 lightSpacePosition = matLightViewToProjection * matWorldToLigthView * vec4(Position, 1.0f);

	vec3 projCoords = lightSpacePosition.xyz / lightSpacePosition.w;
	projCoords = projCoords * 0.5f + 0.5f;

	float closestDepth = texture2D(shadowDepthBuffer, projCoords.xy).r;

	float currentDepth = projCoords.z;

	float shadow = 0.0f;
	float bias = max(0.05f * (1.0f - dot(Normal, dirLights[0].direction)), 0.005f);
	if(closestDepth < currentDepth - bias)
		shadow = 0.8f;
	else 
		shadow = 0.0f;

	// keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;

	return shadow;

	//return vec4(vec3(currentDepth), 1.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void main()
{
	// Sample screen buffers
	vec3 Position = texture2D(positionBuffer, vs_outTexcoord).rgb; 
	vec4 NormalBufferColor = texture2D(normalBuffer, vs_outTexcoord);
	vec3 Normal = NormalBufferColor.rgb;
	float Specular = NormalBufferColor.a;
	vec4 Emission = texture2D(emissiveBuffer, vs_outTexcoord);

	vec4 Albedo = texture2D(albedoBuffer, vs_outTexcoord);

	// ------------------------ Point Light Illuminance ------------------- 
	// Diffuse
	vec4 DiffusePoint = vec4(0,0,0,1);
	vec3 LightDir = vec3(0,0,0);
	float NdotLPoint = 0.0f;
	float atten = 0.0f;
	
	for(int i = 0 ; i < numPointLights ; ++i)
	{
		LightDir = normalize(Position - pointLights[i].position);
		float dist = length(LightDir);
		float r = pointLights[i].radius;
	
		// ref : https://imdoingitwrong.wordpress.com/2011/01/31/light-attenuation/
		atten = 1 / dist; //(1 + ((2/r)*dist) + ((1/r*r)*(dist*dist)));
		
		// diffuse
		NdotLPoint = max(dot(Normal, -LightDir), 0);
	
		// accumulate...
		DiffusePoint += pointLights[i].color * atten * NdotLPoint * pointLights[i].intensity;
	}
	
	// Specular
	vec4 SpecularPoint = vec4(0,0,0,1);
	vec3 reflVectorPoint = vec3(0,0,0);
	float RdotVPoint = 0.0f;
	vec3 viewDir = normalize(cameraPosition - Position);
	
	for(int i = 0 ; i < numPointLights ; ++i)
	{
		LightDir = normalize(Position - pointLights[i].position);
		float dist = length(LightDir);
		float r = pointLights[i].radius;
	
		// ref : https://imdoingitwrong.wordpress.com/2011/01/31/light-attenuation/
		atten = 1 / dist; //(1 + ((2/r)*dist) + ((1/r*r)*(dist*dist)))
	
		// specular
		reflVectorPoint = normalize(reflect(LightDir, Normal));
		RdotVPoint = pow(clamp(dot(reflVectorPoint, viewDir), 0, 1), 32);
	
		// accumulate...
		SpecularPoint += pointLights[i].color * atten * RdotVPoint;
	}
	// ------------------------ End of Point Light Illuminance ------------------- 

	// ------------------------ Directional Light Illuminance ------------------- 
	// Diffuse
	vec4 DiffuseDirectional = vec4(0,0,0,1);
	float NdotLDir = 0.0f;
	
	for(int i = 0 ; i < numDirectionalLights ; ++i)
	{
		// diffuse
		NdotLDir = max(dot(Normal, -dirLights[i].direction), 0);
	
		// accumulate...
		DiffuseDirectional += dirLights[i].color * NdotLDir * dirLights[i].intensity;
	}
	
	// Specular
	vec4 SpecularDirectional = vec4(0,0,0,1);
	vec3 reflVectorDir = vec3(0,0,0);
	float RdotVDir = 0.0f;
	
	for(int i = 0 ; i < numDirectionalLights ; ++i)
	{
		LightDir = dirLights[i].direction;
	
		// specular
		reflVectorDir = normalize(reflect(LightDir, Normal));
		RdotVDir = pow(clamp(dot(reflVectorDir, viewDir), 0, 1), 32);
	
		// accumulate...
		SpecularDirectional += dirLights[i].color * RdotVDir;
	}
	// ------------------------ End of Point Light Illuminance ------------------- 

	// Reflection
	vec4 Reflection = vec4(0);
	vec3 viewReflection = normalize(reflect(viewDir, Normal));
	Reflection = texture(texture_skybox, viewReflection);
	Reflection *= 1.0f;

	// Shadow
	float Shadow = readShadowMap(Position, Normal, viewDir);
	vec4 ShadowColor = vec4(vec3(1.0f - Shadow), 1.0f);

	outColor = Albedo * ShadowColor + ShadowColor * ((DiffusePoint + DiffuseDirectional) + (SpecularPoint * Specular + SpecularDirectional * Specular)) + Reflection * Specular;
	//outColor = Albedo + ((DiffusePoint + DiffuseDirectional) + (SpecularPoint * Specular + SpecularDirectional * Specular)) + Reflection * Specular;

	// Always add Emission color to bright buffer!
	brightColor = Emission * 3.0f;
}