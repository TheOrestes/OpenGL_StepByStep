
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
// Other Uniforms
//---------------------------------------------------------------------------------------
uniform mat4		matWorld;
uniform sampler2D 	texture_diffuse;
uniform sampler2D	texture_specular;
uniform sampler2D 	texture_normal;
uniform samplerCube texture_skybox;

uniform vec3 cameraPosition;
uniform vec3 lightDirection = vec3(0,0,-1);

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
	
	// Diffuse
	float NdotL = clamp(dot(normalize(vs_outNormal), -lightDirection), 0, 1);
	Diffuse = vec4(NdotL, NdotL, NdotL, 1);

	// Specular
	vec3 viewDir = normalize(cameraPosition - vs_outPosition);
	vec3 reflVector = normalize(reflect(lightDirection, vs_outNormal));
	float RdotV = pow(clamp(dot(reflVector, viewDir), 0, 1), 32);
	Specular = vec4(RdotV, RdotV, RdotV, 1);
	
	// control specular based on specular texture 
	Specular *= specColor;

	// Reflection
	vec3 viewReflection = normalize(reflect(viewDir, vs_outNormal));
	Reflection = texture(texture_skybox, viewReflection);

	// control reflection
	Reflection *= 0.3;

	// Final Accumulation
	outColor = Ambient * Diffuse + Specular + Reflection;
}