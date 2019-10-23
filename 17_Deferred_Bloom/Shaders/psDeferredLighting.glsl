
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

uniform samplerCube	texture_skybox;

uniform vec3 cameraPosition;

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
	vec3 reflVector = vec3(0,0,0);
	float RdotV = 0.0f;
	vec3 viewDir = normalize(cameraPosition - Position);
	
	for(int i = 0 ; i < numPointLights ; ++i)
	{
		LightDir = normalize(Position - pointLights[i].position);
		float dist = length(LightDir);
		float r = pointLights[i].radius;
	
		// ref : https://imdoingitwrong.wordpress.com/2011/01/31/light-attenuation/
		atten = 1 / dist; //(1 + ((2/r)*dist) + ((1/r*r)*(dist*dist)))
	
		// specular
		reflVector = normalize(reflect(LightDir, Normal));
		RdotV = pow(clamp(dot(reflVector, viewDir), 0, 1), 32);
	
		// accumulate...
		SpecularPoint += pointLights[i].color * atten * RdotV;
	}
	// ------------------------ End of Point Light Illuminance ------------------- 

	// Reflection
	vec4 Reflection = vec4(0);
	vec3 viewReflection = normalize(reflect(viewDir, Normal));
	Reflection = texture(texture_skybox, viewReflection);
	Reflection *= 1.0f;

	outColor = Albedo + DiffusePoint + SpecularPoint * Specular + Reflection * Specular;

	// Always add Emission color to bright buffer!
	brightColor = Emission * 3.0f;
}