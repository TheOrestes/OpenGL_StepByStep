
#version 400

#define PI 3.14159265359

////////////////////////////////////////////////////////////////////////////////////////////////////
layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 brightColor;

in vec2 vs_outTexcoord;

////////////////////////////////////////////////////////////////////////////////////////////////////
uniform sampler2D positionBuffer;
uniform sampler2D normalBuffer;
uniform sampler2D albedoBuffer;
uniform sampler2D emissiveBuffer;
uniform sampler2D maskBuffer;
uniform sampler2D shadowDepthBuffer;

uniform samplerCube	texture_skybox;

// Camera related
uniform vec3 cameraPosition;
uniform mat4 cameraViewMat;
uniform mat4 cameraProjMat;

// light/shadow related
uniform mat4 matWorldToLigthView;
uniform mat4 matLightViewToProjection;

// Bloom
uniform float fBloomThreshold;

//---------------------------------------------------------------------------------------------------------------------
// Point Lights
//---------------------------------------------------------------------------------------------------------------------
#define MAX_POINT_LIGHTS 8

uniform int			numPointLights;		// number of point lights in the scene
struct PointLight
{
	float radius;
	float intensity;
	vec3 position;
	vec3 color;
};

uniform PointLight pointLights[MAX_POINT_LIGHTS];

//---------------------------------------------------------------------------------------------------------------------
// Directional Lights
//---------------------------------------------------------------------------------------------------------------------
#define MAX_DIRECTIONAL_LIGHTS 8

uniform int			numDirectionalLights;		// number of point lights in the scene
struct DirectionalLight
{
	vec3 direction;
	vec3 color;
	float intensity;
};

uniform DirectionalLight dirLights[MAX_POINT_LIGHTS];

//---------------------------------------------------------------------------------------------------------------------
// Read from Shadow Map
//---------------------------------------------------------------------------------------------------------------------
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
		shadow = 0.5f;
	else 
		shadow = 0.0f;

	// keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;

	return shadow;
}

//---------------------------------------------------------------------------------------------------------------------
// Normal Distribution Function (N) - GGX Based
// in - Vertex Normal, Half Vector, Roughness
// out - Float result
//---------------------------------------------------------------------------------------------------------------------
float NormalDistributionFunction_GGX(vec3 Normal, vec3 Half, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(Normal, Half), 0);
	float NdotH2 = NdotH * NdotH;

	float Nr = a2;
	float Dr = (NdotH2 * (a2 - 1.0f) + 1.0f);
	Dr = PI * Dr * Dr;

	return Nr / max(Dr, 0.001f);	// prevent divide by zero for roughness=0 or NdotH=0 case!
}

//---------------------------------------------------------------------------------------------------------------------
// Geometry Term (G) - GGX Based
// in - Vertex Normal, View Vector, Light Direction, Roughness
// out - Float result
//---------------------------------------------------------------------------------------------------------------------
float Geometry_SchlickGGX(float NdotV, float roughness)
{
	float r = roughness + 1.0f;
	float k = (r * r) / 8.0f;

	float Nr = NdotV;
	float Dr = NdotV * (1.0f - k) + k;

	return Nr / Dr;
}

float GeometryFunction_Smith(vec3 Normal, vec3 View, vec3 LightDir, float roughness)
{
	float NdotV = max(dot(Normal, View), 0.0f);
	float NdotL = max(dot(Normal, LightDir), 0.0f);
	float ggx2 = Geometry_SchlickGGX(NdotV, roughness);
	float ggx1 = Geometry_SchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

//---------------------------------------------------------------------------------------------------------------------
// Fresnel Term (F) 
// in - cosine of angle between Half & View, F0 - surface reflection at zero / direct incidence angle
// out - Vec3 result
//---------------------------------------------------------------------------------------------------------------------
vec3 Fresnel_Schilck(float costTheta, vec3 F0)
{
	return F0 + (1.0f - F0) * pow(1.0f - costTheta, 5.0f);
}

//---------------------------------------------------------------------------------------------------------------------
// Point Lights Illuminance
// in - Vertex Position, Vertex Normal, Roughness
// out - Diffuse & Specular due to point light
//---------------------------------------------------------------------------------------------------------------------
void PointLightIlluminance(vec3 Position, vec3 Normal, vec3 Albedo, float roughness, float Metallic, out vec3 Lo)
{
	vec3 viewDir = normalize(cameraPosition - Position);
	
	for(int i = 0 ; i < numPointLights ; ++i)
	{
		vec3 LightDir = normalize(pointLights[i].position - Position);
		float dist = length(LightDir);
		float r = pointLights[i].radius;
	
		float atten = max(1 / (dist * dist) - 1 / (r * r), 0.0f); 
		
		// diffuse
		float NdotL = max(dot(Normal, LightDir), 0);
		
		// specular
		vec3 Half = normalize(viewDir + LightDir);
		
		vec3 F0 = vec3(0.04f);
		F0 = mix(F0, Albedo, Metallic);

		// Cook-Torrance BRDF
		float NDF = NormalDistributionFunction_GGX(Normal, Half, roughness);
		float G = GeometryFunction_Smith(Normal, viewDir, LightDir, roughness);
		vec3 F = Fresnel_Schilck(max(dot(Half, viewDir), 0.0f), F0);
		
		// Energy conservation between diffuse & Specular since they can't be above 1.
		vec3 Ks = F;
		vec3 Kd = vec3(1) - Ks;
		Kd *= 1.0f - Metallic;

		vec3 Numerator = NDF * G * F;
		float Denominator = 4.0f * max(dot(Normal, viewDir), 0.0f) * max(dot(Normal, LightDir), 0.0f);
		vec3 Specular = Numerator / max(Denominator, 0.001f);

		// accumulate...
		Lo += (Kd * Albedo / PI + Specular) * pointLights[i].color * pointLights[i].intensity * atten * NdotL;
	}
}

//---------------------------------------------------------------------------------------------------------------------
// Directional Light Illuminance
// in - Vertex Position, Vertex Normal, Roughness
// out - Diffuse & Specular due to point light
//---------------------------------------------------------------------------------------------------------------------
void DirectionalLightIlluminance(vec3 Position, vec3 Normal, vec3 Albedo, float roughness, float Metallic, out vec3 Lo)
{
	vec3 viewDir = normalize(cameraPosition - Position);
	
	for(int i = 0 ; i < numDirectionalLights ; ++i)
	{
		vec3 LightDir = dirLights[i].direction;

		// diffuse
		float NdotL = max(dot(Normal, LightDir), 0);
		
		// specular
		vec3 Half = normalize(viewDir + LightDir);
		
		vec3 F0 = vec3(0.04f);
		F0 = mix(F0, Albedo, Metallic);

		// Cook-Torrance BRDF
		float NDF = NormalDistributionFunction_GGX(Normal, Half, roughness);
		float G = GeometryFunction_Smith(Normal, viewDir, LightDir, roughness);
		vec3 F = Fresnel_Schilck(max(dot(Half, viewDir), 0.0f), F0);
		
		// Energy conservation between diffuse & SpecularDir
		vec3 Ks = F;
		vec3 Kd = vec3(1) - Ks;
		Kd *= 1.0f - Metallic;

		vec3 Numerator = NDF * G * F;
		float Denominator = 4.0f * max(dot(Normal, viewDir), 0.0f) * max(dot(Normal, LightDir), 0.0f);
		vec3 Specular = Numerator / max(Denominator, 0.001f);

		// accumulate...
		Lo += (Kd * Albedo / PI + Specular) * dirLights[i].color * dirLights[i].intensity * NdotL;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void main()
{
	// Sample screen buffers
	vec3 Position = texture2D(positionBuffer, vs_outTexcoord).rgb; 
	vec4 NormalBufferColor = texture2D(normalBuffer, vs_outTexcoord);
	vec3 Normal = NormalBufferColor.rgb;
	float Height = NormalBufferColor.a;
	vec4 Emission = texture2D(emissiveBuffer, vs_outTexcoord);
	vec4 Albedo = 2.5f * texture2D(albedoBuffer, vs_outTexcoord);
	vec3 Mask = texture2D(maskBuffer, vs_outTexcoord).rgb;

	vec3 Lo_Dir = vec3(0);
	vec3 Lo_Point = vec3(0);

	DirectionalLightIlluminance(Position, Normal, Albedo.rgb, Mask.r, Mask.g, Lo_Dir);
	PointLightIlluminance(Position, Normal, Albedo.rgb, Mask.r, Mask.g, Lo_Point);

	vec3 Lo = Lo_Dir + Lo_Point;

	// Reflection
	vec4 Reflection = vec4(0);
	vec3 viewDir = normalize(cameraPosition - Position);
	vec3 viewReflection = normalize(reflect(viewDir, Normal));
	Reflection = texture(texture_skybox, viewReflection);
	Reflection *= 0.02f;

	// Shadow
	float Shadow = readShadowMap(Position, Normal, viewDir);
	vec4 ShadowColor = vec4(vec3(1.0f - Shadow), 1.0f);

	// Occlusion
	vec4 Occlusion = vec4(vec3(Mask.b), 1.0f);

	//outColor = Albedo + ShadowColor * (Diffuse + Specular) + Reflection;
	outColor = vec4(Lo,1) + Albedo * Occlusion + Reflection;
	//outColor = vec4(Mask,1);

	// Always add Emission color to bright buffer!
	float brightness = dot(outColor.rgb, vec3(0.2126f, 0.7152f, 0.0722f));
	if(brightness > fBloomThreshold)
		Emission += vec4(Lo, 1);

	brightColor = Emission;
}