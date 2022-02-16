
#version 400

layout (location = 0) out vec4 outColor;

in vec3 vs_outPosition;
in vec2 vs_outUV;
in vec3 vs_outNormal;

//---------------------------------------------------------------------------------------
// Material Properties
//---------------------------------------------------------------------------------------
struct Material
{
	vec4 Albedo;
	vec4 Roughness;
	vec4 Metallic;
};

uniform Material material;

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
// Other Uniforms
//---------------------------------------------------------------------------------------
uniform mat4		matWorld;
uniform sampler2D 	texture_diffuse;
uniform sampler2D	texture_specular;
uniform sampler2D 	texture_normal;
uniform samplerCube texture_skybox;

uniform vec3 cameraPosition;

void main()
{
	vec4 Ambient = vec4(0);
	vec4 Diffuse = vec4(0);
	vec4 Specular = vec4(0);
	vec4 Reflection = vec4(0);

	// texture look-ups
	vec4 baseColor = texture(texture_diffuse, vs_outUV);
	vec4 specColor = texture(texture_specular, vs_outUV);

	// Ambient
	Ambient	= material.Albedo * baseColor;
	
	// ------------------------ Point Light Illuminance ------------------- 
	// Diffuse
	vec4 DiffusePoint = vec4(0,0,0,1);
	vec3 LightDir = vec3(0,0,0);
	float NdotLPoint = 0.0f;
	float atten = 0.0f;
	
	for(int i = 0 ; i < numPointLights ; ++i)
	{
		LightDir = normalize(vs_outPosition - pointLights[i].position);
		float dist = length(LightDir);
		float r = pointLights[i].radius;

		// ref : https://imdoingitwrong.wordpress.com/2011/01/31/light-attenuation/
		atten = 1 / dist; //(1 + ((2/r)*dist) + ((1/r*r)*(dist*dist)));
		
		// diffuse
		NdotLPoint = max(dot(vs_outNormal, -LightDir), 0);

		// accumulate...
		DiffusePoint += pointLights[i].color * atten * NdotLPoint * pointLights[i].intensity;
	}

	// Specular
	vec4 SpecularPoint = vec4(0,0,0,1);
	vec3 reflVector = vec3(0,0,0);
	float RdotV = 0.0f;
	vec3 viewDir = normalize(cameraPosition - vs_outPosition);

	for(int i = 0 ; i < numPointLights ; ++i)
	{
		LightDir = normalize(vs_outPosition - pointLights[i].position);
		float dist = length(LightDir);
		float r = pointLights[i].radius;

		// ref : https://imdoingitwrong.wordpress.com/2011/01/31/light-attenuation/
		atten = 1 / dist; //(1 + ((2/r)*dist) + ((1/r*r)*(dist*dist)))

		// specular
		reflVector = normalize(reflect(LightDir, vs_outNormal));
		RdotV = pow(clamp(dot(reflVector, viewDir), 0, 1), 32);

		// accumulate...
		SpecularPoint += pointLights[i].color * atten * RdotV;
	}
	// ------------------------ End of Point Light Illuminance ------------------- 
	
	// control specular based on specular texture 
	SpecularPoint *= specColor;

	// Reflection
	vec3 viewReflection = normalize(reflect(viewDir, vs_outNormal));
	Reflection = texture(texture_skybox, viewReflection);

	// control reflection
	Reflection *= 0.3;

	// Final Accumulation
	outColor = Ambient * DiffusePoint + SpecularPoint;// + Reflection;
}