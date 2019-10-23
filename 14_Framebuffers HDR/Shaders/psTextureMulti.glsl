
#version 400

layout (location = 0) out vec4 outColor;

in vec3 vs_outPosition;
in vec2 vs_outUV;

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

void main()
{
	vec4 Ambient = vec4(0);

	// Final Color components...
	//Ambient	= material.Albedo * texture(texture_diffuse, vs_outUV);
	//Ambient = texture(texture_specular, vs_outUV);
	Ambient = texture(texture_normal, vs_outUV);
	
	outColor = Ambient;
}