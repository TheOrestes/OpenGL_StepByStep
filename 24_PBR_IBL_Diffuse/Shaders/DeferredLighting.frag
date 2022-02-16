
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
uniform sampler2D skyboxBuffer;
uniform sampler2D objectIDBuffer;
uniform sampler2D shadowDepthBuffer;

uniform samplerCube texture_irradiance;

// Camera related
uniform vec3 cameraPosition;
uniform mat4 cameraViewMat;
uniform mat4 cameraProjMat;

// light/shadow related
uniform mat4 matWorldToLigthView;
uniform mat4 matLightViewToProjection;

// Bloom
uniform float fBloomThreshold;

// Debug
uniform int channelID;

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
// Linearize color
//---------------------------------------------------------------------------------------------------------------------
vec3 LinearizeColor(vec3 color)
{
	return pow(color, vec3(2.2f));
}

//---------------------------------------------------------------------------------------------------------------------
// Read from Shadow Map
//---------------------------------------------------------------------------------------------------------------------
float closestDepth = 0;
float readShadowMap(vec3 Position, vec3 Normal, vec3 viewDir)
{
	vec4 lightSpacePosition = matLightViewToProjection * matWorldToLigthView * vec4(Position, 1.0f);

	vec3 projCoords = lightSpacePosition.xyz / lightSpacePosition.w;
	projCoords = projCoords * 0.5f + 0.5f;

	closestDepth = texture2D(shadowDepthBuffer, projCoords.xy).r;

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
vec3 Fresnel_Schlick(float costTheta, vec3 F0)
{
	return F0 + (1.0f - F0) * pow(1.0f - costTheta, 5.0f);
}

//---------------------------------------------------------------------------------------------------------------------
// Fresnel Term (F) with roughness consideration for Indirect Diffuse calculations
// in - cosine of angle between Half & View, F0 - surface reflection at zero / direct incidence angle
// out - Vec3 result
//---------------------------------------------------------------------------------------------------------------------
vec3 Fresnel_Schlick_Roughness(float costTheta, vec3 F0, float roughness)
{
	return F0 + (max(vec3(1.0f - roughness), F0) - F0) * pow(1.0f - costTheta, 5.0f);
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
		
		vec3 F0 = vec3(0.6f);
		F0 = mix(F0, Albedo, Metallic);

		// Cook-Torrance BRDF
		float NDF = NormalDistributionFunction_GGX(Normal, Half, roughness);
		float G = GeometryFunction_Smith(Normal, viewDir, LightDir, roughness);
		vec3 F = Fresnel_Schlick(max(dot(Half, viewDir), 0.0f), F0);
		
		// Energy conservation between diffuse & Specular since they can't be above 1.
		vec3 Ks = F;
		vec3 Kd = vec3(1) - Ks;
		Kd *= 1.0f - Metallic;

		vec3 Numerator = NDF * G * F;
		float Denominator = 4.0f * max(dot(Normal, viewDir), 0.0f) * max(dot(Normal, LightDir), 0.0f);
		vec3 Specular = Numerator / max(Denominator, 0.001f);

		// accumulate...
		Lo += (Kd * Albedo / PI + Ks * Specular) * pointLights[i].color * pointLights[i].intensity * atten * NdotL;
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
		
		vec3 F0 = vec3(0.6f);
		F0 = mix(F0, Albedo, Metallic);

		// Cook-Torrance BRDF
		float NDF = NormalDistributionFunction_GGX(Normal, Half, roughness);
		float G = GeometryFunction_Smith(Normal, viewDir, LightDir, roughness);
		vec3 F = Fresnel_Schlick(max(dot(Half, viewDir), 0.0f), F0);
		
		// Energy conservation between diffuse & SpecularDir
		vec3 Ks = F;
		vec3 Kd = vec3(1) - Ks;
		Kd *= 1.0f - Metallic;

		vec3 Numerator = NDF * G * F;
		float Denominator = 4.0f * max(dot(Normal, viewDir), 0.0f) * max(dot(Normal, LightDir), 0.0f);
		vec3 Specular = Numerator / max(Denominator, 0.001f);

		// accumulate...
		Lo += (Kd * Albedo / PI + Ks * Specular) * dirLights[i].color * dirLights[i].intensity * NdotL;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void main()
{
	// Sample screen buffers
	vec4 PosBuffer = texture2D(positionBuffer, vs_outTexcoord); 
	vec3 Position = PosBuffer.rgb;

	vec4 NormalBufferColor = texture2D(normalBuffer, vs_outTexcoord);
	vec3 Normal = NormalBufferColor.rgb;
	float Height = NormalBufferColor.a;
	vec3 Emission = texture2D(emissiveBuffer, vs_outTexcoord).rgb;
	vec3 Albedo = 2.0f * (texture2D(albedoBuffer, vs_outTexcoord).rgb);
	vec3 Mask = texture2D(maskBuffer, vs_outTexcoord).rgb;
	vec3 Skybox = texture2D(skyboxBuffer, vs_outTexcoord).rgb;
	vec3 shadowDepth = vec3(texture2D(shadowDepthBuffer, vs_outTexcoord).r);
	vec3 ObjectID = texture2D(objectIDBuffer, vs_outTexcoord).rgb;

	float GeometryMask = Mask.b;

	// Calculate Camera view direction & reflection vector!
	vec3 viewDir = normalize(cameraPosition - Position);
	vec3 viewReflection = normalize(reflect(-viewDir, Normal));

	// Shadow
	float Shadow = readShadowMap(Position, Normal, viewDir);
	vec3 ShadowColor = vec3(1-Shadow);

	// consider shadow contribution!
	Albedo *= ShadowColor;

	// Extract for readability!
	float Roughness = Mask.r;
	float Metallic = Mask.g;
	float Occl = Mask.b;

	vec3 Lo_Dir = vec3(0);
	vec3 Lo_Point = vec3(0);

	DirectionalLightIlluminance(Position, Normal, Albedo.rgb, Roughness, Metallic, Lo_Dir);
	PointLightIlluminance(Position, Normal, Albedo.rgb, Roughness, Metallic, Lo_Point);

	vec3 Lo_Direct = Lo_Dir + Lo_Point;

	// Occlusion
	vec3 Occlusion = vec3(Occl);

	// Just like direct lighting has diffuse & specular component, even indirect lighting has it! 
	// It is necessary to weigh both accordingly using Fresnel equation.
	vec3 F0 = vec3(0.04f);
	F0 = mix(F0, Albedo, Metallic);
	vec3 Ks = Fresnel_Schlick_Roughness(max(dot(Normal, viewDir), 0.0f), F0, Roughness);
	vec3 Kd = 1.0f - Ks;
	Kd *= 1.0f - Metallic;

		//---------- Indirect Diffuse
	vec3 Irradiance = vec3(texture(texture_irradiance, Normal));
	vec3 IndirectDiffuse = Kd * Irradiance * (Albedo / PI);
	
	vec3 Lo_Indirect = (IndirectDiffuse) * Occlusion;
	
	vec3 Lo = vec3(0);

	// determine object id from RGB texture
	vec3 redChannel = vec3(1,0,0);		// Static objects
	vec3 greenChannel = vec3(0,1,0);	// Skybox

	if(dot(redChannel, ObjectID) == 1)
		Lo = Lo_Direct + Lo_Indirect ;
	else if(dot(greenChannel, ObjectID) == 1)
		Lo = Skybox;

	// Always add Emission color to bright buffer!
	// do not consider HDRI map into brightness calculations!
	float brightness = dot((Lo_Direct).rgb, vec3(0.2126f, 0.7152f, 0.0722f));
	if(brightness > fBloomThreshold)
		Emission += (Lo_Direct);

	brightColor = vec4(Emission,1);

	// Decide which channel to show during final output!
	if(channelID == 0)
		outColor = vec4(Lo,1);
	else if(channelID == 1)
		outColor = vec4(Lo_Direct, 1);
	else if(channelID == 2)
		outColor = vec4(IndirectDiffuse, 1);
	else if(channelID == 3)
		outColor = vec4(shadowDepth, 1);
	else if(channelID == 4)
		outColor = vec4(Albedo, 1);
	else if(channelID == 5)
		outColor = vec4(Position, 1);
	else if(channelID == 6)
		outColor = vec4(Normal, 1);
	else if(channelID == 7)
		outColor = vec4(Emission, 1);
	else if(channelID == 8)
		outColor = vec4(vec3(Roughness), 1);
	else if(channelID == 9)
		outColor = vec4(vec3(Metallic), 1);
	else if(channelID == 10)
		outColor = vec4(Occlusion, 1);
	else if(channelID == 11)
		outColor = vec4(Skybox, 1);
	else if(channelID == 12)
		outColor = vec4(ObjectID, 1);
}