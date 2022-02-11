
#version 400

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec3 gAlbedo;
layout (location = 3) out vec3 gEmission;

in vec3 vs_outPosition;
in vec2 vs_outUV;
in mat3 vs_outTBN;
in vec3 vs_outBarycentric;

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
uniform bool		hasDiffuse = false;
uniform bool		hasSpecular = false;
uniform bool		hasNormal = false;
uniform bool		hasEmissive = false;
uniform sampler2D 	texture_diffuse;
uniform sampler2D	texture_specular;
uniform sampler2D 	texture_normal;
uniform sampler2D	texture_emissive;

uniform vec3 cameraPosition;

/////////////////////////////////////////////////////////////////////////////////////////
float edgeFactor()
{
	vec3 d = fwidth(vs_outBarycentric);
	vec3 f = smoothstep(vec3(0), d * 0.75f, vs_outBarycentric);
	return min(min(f.x, f.y), f.z);
}

void main()
{
	vec4 Ambient = vec4(0);
	vec4 Diffuse = vec4(0);
	vec4 Specular = vec4(0);

	// texture look-ups
	vec4 baseColor = hasDiffuse ? texture(texture_diffuse, vs_outUV) : vec4(1);
	vec4 specColor = hasSpecular ? texture(texture_specular, vs_outUV) : vec4(0);
	vec4 emissiveColor = hasEmissive ? texture(texture_emissive, vs_outUV) : vec4(0);

	// Extract normals, get them from [0,1] to [-1,1] range
	vec3 texNormal = hasNormal ? normalize(texture(texture_normal, vs_outUV) * 2.0 - 1.0).rgb : vec3(1);

	// use TBN matrix to transform these Tangent space normals to World Space
	vec3 Normal = normalize(vs_outTBN * texNormal);

	// ! write World Space position into Position buffer
	gPosition = vs_outPosition;

	// ! Write into Normal Buffer : Normals(RGB) + Specular(A)
	gNormal.rgb = Normal;
	gNormal.a = specColor.r;

	// Ambient
	Ambient	= material.Albedo * baseColor;

	// ! write into Albedo buffer
	//if(any(lessThan(vs_outBarycentric, vec3(0.02f))))
	//	gAlbedo = vec3(0,1,0);
	//else
		
	gAlbedo = mix(vec3(0.8f,1.0f,0.0f), Ambient.rgb, edgeFactor());

	// ! Write into Emissive buffer
	gEmission = emissiveColor.rgb;
}