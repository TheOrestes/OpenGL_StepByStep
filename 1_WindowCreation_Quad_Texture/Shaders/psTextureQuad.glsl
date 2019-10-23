
#version 400

layout (location = 0) out vec4 outColor;

in vec3 vs_outPosition;
in vec2 vs_outUV;

//---------------------------------------------------------------------------------------
// Other Uniforms
//---------------------------------------------------------------------------------------
uniform mat4		matWorld;
uniform sampler2D 	texture_diffuse;

void main()
{
	vec4 Ambient = vec4(0);

	// Final Color components...
	Ambient	= texture(texture_diffuse, vs_outUV);
	
	outColor = Ambient;
}