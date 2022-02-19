
#version 400

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec3 gAlbedo;

in vec3 vs_outPosition;
in vec2 vs_outUV;
in mat3 vs_outTBN;

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

uniform vec3 cameraPosition;

void main()
{
	vec4 Ambient = vec4(0);
	vec4 Diffuse = vec4(0);
	vec4 Specular = vec4(0);

	// texture look-ups
	vec4 baseColor = texture(texture_diffuse, vs_outUV);
	vec4 specColor = texture(texture_specular, vs_outUV);

	// Extract normals, get them from [0,1] to [-1,1] range
	vec3 texNormal = normalize(texture(texture_normal, vs_outUV) * 2.0 - 1.0).rgb;

	// use TBN matrix to transform these Tangent space normals to World Space
	vec3 Normal = normalize(vs_outTBN * texNormal);

	// ! write into Position buffer
	gPosition = vs_outPosition;

	// ! Write into Normal Buffer : Normals(RGB) + Specular(A)
	gNormal.rgb = Normal;
	gNormal.a = specColor.r;

	// Ambient
	Ambient	= material.Albedo * baseColor;

	// ! write into Albedo buffer
	gAlbedo = Ambient.rgb;
}