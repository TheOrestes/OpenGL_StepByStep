
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

uniform vec3 lightDirection = vec3(0,0,-1);

void main()
{
	vec4 Ambient = vec4(0);
	vec4 Diffuse = vec4(0);

	// Final Color components...
	Ambient	= texture(texture_diffuse, vs_outUV);
	
	// Dot product!
	float NdotL = clamp(dot(normalize(vs_outNormal), -lightDirection), 0, 1);
	Diffuse = vec4(NdotL, NdotL, NdotL, 1);
	
	outColor = Diffuse;
}