
#version 400

////////////////////////////////////////////////////////////////////////////////////////////////////
in vec2 vs_outTexcoord;
out vec4 outColor;

////////////////////////////////////////////////////////////////////////////////////////////////////
uniform sampler2D colorBuffer;
uniform sampler2D brightBuffer;
uniform float gamma;
uniform float exposure;
uniform bool DoBloom;

////////////////////////////////////////////////////////////////////////////////////////////////////
uniform float weight[9] = float[](0.0625f, 0.125f, 0.0625f, 0.125f, 0.25f, 0.125f, 0.0625f, 0.125f, 0.0625f);

uniform float blurStrength = 0.1f;

////////////////////////////////////////////////////////////////////////////////////////////////////
vec4 blurKernel()
{
	vec2 tex_offset = 1 / textureSize(brightBuffer, 0); // gets size of single texel
	int texLod = 0;
    vec3 result = vec3(0); 

	vec2 direction = vec2(0);

	for(int i = 0 ; i < 8 ; i++)
	{
		if(i%2 == 0)
		{
			direction = vec2(1,0);
			vec2 off1 = tex_offset * direction;
			result += texture2D(brightBuffer, vs_outTexcoord).rgb * 0.29411764705882354;
			result += texture2D(brightBuffer, vs_outTexcoord + off1 ).rgb * 0.35294117647058826;
			result += texture2D(brightBuffer, vs_outTexcoord - off1 ).rgb * 0.35294117647058826;
		}
		else
		{
			direction = vec2(0,1);
			vec2 off1 = tex_offset * direction;
			result += texture2D(brightBuffer, vs_outTexcoord).rgb * 0.29411764705882354;
			result += texture2D(brightBuffer, vs_outTexcoord + off1).rgb * 0.35294117647058826;
			result += texture2D(brightBuffer, vs_outTexcoord - off1).rgb * 0.35294117647058826;
		}
	}

	//for(int i = 2 ; i < 4 ; i++)
	//{
	//	result += textureLod(brightBuffer, vs_outTexcoord + vec2(-tex_offset.x,  tex_offset.y),	i).rgb * weight[0]; 
	//	result += textureLod(brightBuffer, vs_outTexcoord + vec2(0.0f,			 tex_offset.y),	i).rgb * weight[1]; 
	//	result += textureLod(brightBuffer, vs_outTexcoord + vec2(tex_offset.x,	 tex_offset.y),	i).rgb * weight[2]; 
	//	result += textureLod(brightBuffer, vs_outTexcoord + vec2(-tex_offset.x,  0.0f),			i).rgb * weight[3]; 
	//	result += textureLod(brightBuffer, vs_outTexcoord + vec2(0.0f,			 0.0f),			i).rgb * weight[4]; 
	//	result += textureLod(brightBuffer, vs_outTexcoord + vec2(tex_offset.x,	 0.0f),			i).rgb * weight[5]; 
	//	result += textureLod(brightBuffer, vs_outTexcoord + vec2(-tex_offset.x, -tex_offset.y), i).rgb * weight[6]; 
	//	result += textureLod(brightBuffer, vs_outTexcoord + vec2(0.0f,			-tex_offset.y), i).rgb * weight[7]; 
	//	result += textureLod(brightBuffer, vs_outTexcoord + vec2(tex_offset.x,	-tex_offset.y), i).rgb * weight[8]; 
	//}
      
    return vec4(result, 1.0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void main()
{
	// Sample screen buffer
	vec4 screenColor = texture2D(colorBuffer, vs_outTexcoord); 

	// Sample & blur bright buffer
	vec4 brightBlur = blurKernel();

	if(DoBloom)
		screenColor += brightBlur;

	// reinhard tone mapping
	//vec3 mapped = screenColor.rgb / (screenColor.rgb + vec3(1.0f));

	// exposure tone mapping
	vec3 mapped = vec3(1.0f) - exp(-screenColor.rgb * exposure);

	// Gamma correction
	mapped = pow(mapped, vec3(1.0f / gamma));
	outColor = vec4(mapped, 1.0f);
}