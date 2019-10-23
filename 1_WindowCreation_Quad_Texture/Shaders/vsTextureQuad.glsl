
#version 400

layout(location=0) in vec3 in_Position;
layout(location=1) in vec2 in_Texcoord;

out vec3 vs_outPosition;
out vec2 vs_outUV;

// uniforms...
uniform mat4 matWorld;
uniform mat4 matView;
uniform mat4 matProj;

void main()
{
	mat4 WVP = matProj * matView * matWorld;
	
	// position in world space
	vs_outPosition = (matWorld * vec4(in_Position, 1)).xyz;
	
	// pass on UVs
	vs_outUV = in_Texcoord;

	gl_Position = WVP * vec4(in_Position, 1.0);
}