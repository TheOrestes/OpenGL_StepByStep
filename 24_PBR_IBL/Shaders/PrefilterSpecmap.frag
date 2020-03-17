
// Based on https://learnopengl.com/PBR/IBL/Diffuse-irradiance

#version 400

#define PI 3.14159265359

in vec3 localPos;

layout (location = 0) out vec4 outColor;

uniform samplerCube CubemapTexture;
uniform float roughness;

//---------------------------------------------------------------------------------------------------------------------
// Directional Light Illuminance
// in - Vertex Position, Vertex Normal, Roughness
// out - Diffuse & Specular due to point light
//---------------------------------------------------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;

	float NdotH = max(dot(N,H), 0.0f);
	float NdotH2 = NdotH * NdotH;

	float Nr = a2;
	float Dr = (NdotH * (a2 - 1.0f) + 1.0f);
	Dr = PI * Dr * Dr;

	return Nr / Dr;
}

//---------------------------------------------------------------------------------------------------------------------
// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
// efficient VanDerCorpus calculation.
// In nutshell, it's based on Hammersley's uniform random point generator. (Ray tracing from ground up has detailed note)
//---------------------------------------------------------------------------------------------------------------------
float RadicalInverse_VdC(uint bits)
{
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

//---------------------------------------------------------------------------------------------------------------------
// Generate sample vector based on random value
//---------------------------------------------------------------------------------------------------------------------
vec2 Hammersley(uint i, uint N)
{
	return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}

//---------------------------------------------------------------------------------------------------------------------
// GGX Importance sampling
// Instead of uniform or random samples over a hemisphere, we'll generate sample vectors biased towards general reflection
// orientation of miscrosurface half vector (based on roughness).

// We generate random value, use it to generate sample vector in tangent space, then transform it to world space and sample
// scene's radiance!

// For biasing the sample vector towards the specular lobe for surface roughness, we use NDF. 

// in - Vertex Position, Vertex Normal, Roughness
// out - Diffuse & Specular due to point light
//---------------------------------------------------------------------------------------------------------------------
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
	float a = roughness * roughness;
	float phi = 2.0f * PI * Xi.x;
	float cosTheta = sqrt((1.0f - Xi.y) / (1.0f + (a*a - 1.0f) * Xi.y));
	float sinTheta = sqrt(1.0f - cosTheta*cosTheta);

	// from spherical coordinates to cartesian coordinates - half vector
	vec3 H;
	H.x = cos(phi) * sinTheta;
	H.y = sin(phi) * sinTheta;
	H.z = cosTheta;

	// from tangent space to world space
	vec3 up = abs(N.z) < 0.999f ? vec3(0,0,1) : vec3(1,0,0);
	vec3 tangent = normalize(cross(up, N));
	vec3 bitangent = cross(N, tangent);

	vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;

	return normalize(sampleVec);
}

//---------------------------------------------------------------------------------------------------------------------
void main()
{
	vec3 N = normalize(localPos);

	// make simplifying assumption that V = R = N
	vec3 R = N;
	vec3 V = R;

	const uint	SAMPLE_COUNT= 1024u;
	vec3		prefilteredColor = vec3(0);
	float		totalWeight = 0.0f;

	for(uint i = 0u; i < SAMPLE_COUNT ; ++i)
	{
		// generates a sample vector that is biased towards the preferred alignment direction (Importane Sampling) 
		vec2 Xi = Hammersley(i, SAMPLE_COUNT);
		vec3 H = ImportanceSampleGGX(Xi, N, roughness);
		vec3 L = normalize(2.0f * dot(V,H) * H - V);

		float NdotL = max(dot(N,L), 0.0f);
		if(NdotL > 0.0f)
		{
			// sample from the environment's mip  level based on rougness/pdf
			float D = DistributionGGX(N, H, roughness);
			float NdotH = max(dot(N,H), 0.0f);
			float HdotV = max(dot(H,V), 0.0f);
			float pdf = D * NdotH / (4.0f * HdotV) + 0.0001f;

			// to avoid bright dots in specular reflections, we make sure not to sample envMap, but instead sample
			// mip-level of the envMap based on integral's PDF & roughness!
			float resolution = 2048.0f;
			float saTexel = 4.0f * PI / (6.0f * resolution * resolution);
			float saSample = 1.0f / (float(SAMPLE_COUNT) * pdf + 0.0001f);

			float mipLevel = roughness == 0.0f ? 0.0f : 0.5f * log2(saSample / saTexel);

			prefilteredColor += textureLod(CubemapTexture, L, mipLevel).rgb * NdotL;
			totalWeight += NdotL;
		}
	}

	prefilteredColor = prefilteredColor / totalWeight;

	outColor = vec4(prefilteredColor, 1.0f);
}
