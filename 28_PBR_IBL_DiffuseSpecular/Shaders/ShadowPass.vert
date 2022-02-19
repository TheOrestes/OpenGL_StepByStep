
#version 400

layout(location=0) in vec3 in_Position;


// uniforms...
uniform mat4 matWorld;
uniform mat4 matWorldToLightView;
uniform mat4 matLightViewToProjection;

void main()
{	
	mat4 matLightSpace = matLightViewToProjection* matWorldToLightView;

	gl_Position = matLightSpace * (matWorld * vec4(in_Position, 1.0));
}