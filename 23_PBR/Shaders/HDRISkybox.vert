
#version 400

layout(location=0) in vec3 in_Position;

out vec3 vs_outTex;

// uniforms
uniform mat4 matView;
uniform mat4 matProj;

void main()
{
	mat4 wvp = matProj * matView;;
	vec4 Pos = wvp * vec4(in_Position, 1.0f);
	gl_Position = Pos.xyww; 

	vs_outTex = in_Position;
}