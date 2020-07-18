
#version 400

layout(location=0) in vec4 in_Position;
layout(location=1) in vec4 in_Velocity;
layout(location=2) in vec4 in_Acceleration;
layout(location=3) in vec4 in_Color;

out vec3 vs_outPosition;
out float vs_outLiftTime;
out vec4 vs_outVelocity;
out vec4 vs_outAcceleration;
out vec4 vs_outColor;

// uniforms...
uniform mat4 matWorld;
uniform mat4 matView;
uniform mat4 matProj;

void main()
{	
	mat4 WVP = matProj * matView * matWorld;

	gl_Position = WVP * vec4(in_Position.xyz, 1.0);
	vs_outLiftTime = in_Position.w;
	vs_outVelocity = in_Velocity;
	vs_outAcceleration = in_Acceleration;
	vs_outColor = in_Color;

	gl_PointSize = 1.0f;
}