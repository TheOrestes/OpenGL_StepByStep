
#version 400

////////////////////////////////////////////////////////////////////////////////////////////////////
layout (location = 0) out vec4 outColor;

in vec2 vs_outTexcoord;

////////////////////////////////////////////////////////////////////////////////////////////////////
uniform sampler2D debugBuffer;


////////////////////////////////////////////////////////////////////////////////////////////////////
void main()
{
	// Sample screen buffers
	vec3 debugColor = texture2D(debugBuffer, vs_outTexcoord).rgb; 
	
	outColor = vec4(debugColor, 1.0f);
}