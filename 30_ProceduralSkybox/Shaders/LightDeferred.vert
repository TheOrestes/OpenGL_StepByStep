
#version 400

layout(location=0) in vec3 in_Position;
layout(location=1) in vec4 in_Color;

out vec3 vs_outPosition;
out vec4 vs_outColor;

// uniforms...
uniform mat4 matWorld;
uniform mat4 matView;
uniform mat4 matProj;
uniform vec3 lightColor;

void main()
{
	// Output position in World Space
	// position in world space
	vs_outPosition = (matWorld * vec4(in_Position, 1)).xyz;
	vs_outColor = vec4(lightColor,1);
	
	mat4 WVP = matProj * matView * matWorld;

	gl_Position = WVP * vec4(in_Position, 1.0);
}