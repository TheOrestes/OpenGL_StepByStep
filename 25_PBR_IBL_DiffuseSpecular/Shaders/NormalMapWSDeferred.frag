
#version 400

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec3 gAlbedo;
layout (location = 3) out vec3 gEmission;
layout (location = 4) out vec3 gMask;
layout (location = 5) out vec3 gSkybox;
layout (location = 6) out vec3 gObjectID;

in vec3 vs_outPosition;
in vec2 vs_outUV;
in mat3 vs_outTBN;
in vec3 vs_outNormal;
in vec3 vs_outBarycentric;

//---------------------------------------------------------------------------------------
// Material Properties
//---------------------------------------------------------------------------------------
struct Material
{
	vec4  Albedo;
	vec4  Emission;
	float Roughness;
	float Metallic;
	float Occlusion;
	float Height;
};

uniform Material material;

//---------------------------------------------------------------------------------------
// Other Uniforms
//---------------------------------------------------------------------------------------
uniform mat4		matWorld;
uniform bool		hasDiffuse = false;
uniform bool		hasMask = false;
uniform bool		hasNormal = false;
uniform bool		hasEmissive = false;
uniform sampler2D 	texture_diffuse;
uniform sampler2D	texture_mask;
uniform sampler2D 	texture_normal;
uniform sampler2D	texture_emissive;

uniform vec3		cameraPosition;

uniform vec3		wireframeColor = vec3(0.0f, 0.0f, 0.0f);
uniform float		wireframeWidth = 0.75f;

uniform bool		useD3DNormals = false;

const float nearPlane = 1.0f;
const float farPlane = 1000.0f;

/////////////////////////////////////////////////////////////////////////////////////////
float edgeFactor()
{
	vec3 d = fwidth(vs_outBarycentric);
	vec3 f = smoothstep(vec3(0), d * wireframeWidth, vs_outBarycentric);
	return min(min(f.x, f.y), f.z);
}

/////////////////////////////////////////////////////////////////////////////////////////
float LinearizeDepth(float depth)
{
	float z = depth * 2.0f - 1.0f;
	return (2.0f * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));
}

/////////////////////////////////////////////////////////////////////////////////////////
vec3 ComputeTexNormal(vec3 worldNormal, vec3 texNormal)
{
	vec3 dPosX = dFdx(vs_outPosition);
	vec3 dPosY = dFdy(vs_outPosition);
	vec2 dTexX = dFdx(vs_outUV);
	vec2 dTexY = dFdy(vs_outUV);

	vec3 normal = normalize(worldNormal);
	vec3 tangent = normalize(dPosX * dTexY.t - dPosY * dTexX.t);
	vec3 binormal = -normalize(cross(normal, tangent));
	mat3 TBN = mat3(tangent, binormal, normal);

	return normalize(TBN * texNormal);
}

/////////////////////////////////////////////////////////////////////////////////////////
void main()
{
	vec4 Ambient = vec4(0);
	vec4 Diffuse = vec4(0);
	vec4 Specular = vec4(0);
	vec3 Mask = vec3(0);

	// texture look-ups
	vec4 baseColor = hasDiffuse ? material.Albedo * texture(texture_diffuse, vs_outUV) : material.Albedo;
	vec4 emissiveColor = hasEmissive ? material.Emission * texture(texture_emissive, vs_outUV) : material.Emission;

	// Extract normals, get them from [0,1] to [-1,1] range
	// Flip green channel for normal if we are using DirectX normal map!
	vec3 texNormal = normalize(texture(texture_normal, vs_outUV) * 2.0 - 1.0).rgb;
	if(useD3DNormals)
	{
		texNormal = vec3(texNormal.r, -texNormal.g, texNormal.b);
	}

	// use TBN matrix to transform these Tangent space normals to World Space
	// if object has normal map, then use tangent space texture normals by transforming then to 
	// world space using TBN matrix. If not, then use already transformed world space normals!
	//vec3 Normal = ComputeTexNormal(vs_outNormal, texNormal);//hasNormal ? normalize(vs_outTBN * texNormal) : vs_outNormal;
	vec3 Normal = hasNormal ? normalize(vs_outTBN * texNormal) : vs_outNormal;

	if(hasMask)
	{
		vec3 maskColor = vec3(texture(texture_mask, vs_outUV));
		Mask = vec3(material.Roughness * maskColor.r, material.Metallic * maskColor.g, material.Occlusion * maskColor.b);
	}
	else
		Mask = vec3(material.Roughness, material.Metallic, material.Occlusion);

	// ! write World Space position into Position buffer & depth value to Alpha channel.
	gPosition = vec4(vs_outPosition, 1.0f);

	// ! Write into Normal Buffer : Normals(RGB) + Specular(A)
	gNormal.rgb = Normal;

	// Ambient
	Ambient	= material.Albedo * baseColor;

	gAlbedo = mix(wireframeColor, Ambient.rgb, edgeFactor());

	gMask = Mask;

	// write black color for skybox!
	gSkybox = vec3(0);

	// set Red color for static objects!
	gObjectID = vec3(1,0,0);

	// ! Write into Emissive buffer
	gEmission = emissiveColor.rgb;
}