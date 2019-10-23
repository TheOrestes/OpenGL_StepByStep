
#version 400

////////////////////////////////////////////////////////////////////////////////////////////////////
in vec2 vs_outTexcoord;
out vec4 outColor;

////////////////////////////////////////////////////////////////////////////////////////////////////
uniform sampler2D colorBuffer;
uniform sampler2D brightBuffer;
uniform float gamma;
uniform float exposure;

////////////////////////////////////////////////////////////////////////////////////////////////////
uniform float weight[5] = float[] (0.06136, 0.24477, 0.38774, 0.24477, 0.06136);

////////////////////////////////////////////////////////////////////////////////////////////////////
vec4 blurKernel()
{
	vec2 tex_offset = 1.0 / textureSize(brightBuffer, 0); // gets size of single texel
	int texLod = 0;
    vec3 result = textureLod(brightBuffer, vs_outTexcoord, texLod).rgb * weight[0]; // current fragment's contribution
      
	for(int i = 1, j = 0, k = 0; i < 8; ++i, ++j, ++k)
    {
		if(i%2 == 0)
		{
			result += textureLod(brightBuffer, vs_outTexcoord - vec2(tex_offset.x * j, 0.0), texLod).rgb * weight[j];
			result += textureLod(brightBuffer, vs_outTexcoord + vec2(tex_offset.x * j, 0.0), texLod).rgb * weight[j];
		}
		else
		{
			result += textureLod(brightBuffer, vs_outTexcoord + vec2(0.0, tex_offset.y * k), texLod).rgb * weight[k];
			result += textureLod(brightBuffer, vs_outTexcoord - vec2(0.0, tex_offset.y * k), texLod).rgb * weight[k];
		}
		
		texLod++;
    }
  
    return vec4(result, 1.0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void main()
{
	// Sample screen buffer
	vec4 screenColor = texture2D(colorBuffer, vs_outTexcoord); 

	// Sample & blur bright buffer
	vec4 brightBlur = blurKernel();

	screenColor += brightBlur;

	// reinhard tone mapping
	// vec3 mapped = screenColor.rgb / (screenColor.rgb + vec3(1.0f));

	// exposure tone mapping
	//vec3 mapped = vec3(1.0f) - exp(-screenColor.rgb * exposure);

	// Gamma correction
	//mapped = pow(mapped, vec3(1.0f / gamma));
	outColor = screenColor; //vec4(mapped, 1.0f);
}