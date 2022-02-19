
#version 400

layout(location=0) in vec3 in_Position;

out vec4 color;

// uniforms...
uniform mat4 matWorld;
uniform mat4 matView;
uniform mat4 matProj;

void main()
{	
	mat4 WVP = matProj * matView * matWorld;

	gl_Position = WVP * vec4(in_Position, 1.0);
}