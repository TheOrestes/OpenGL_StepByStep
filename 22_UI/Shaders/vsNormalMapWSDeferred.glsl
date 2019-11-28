
#version 400

layout(location=0) in vec3 in_Position;
layout(location=1) in vec2 in_UV;
layout(location=2) in vec3 in_Normal;
layout(location=3) in vec3 in_BiNormal;
layout(location=4) in vec3 in_Tangent;
layout(location=5) in vec3 in_Barycentric;

out vec3 vs_outPosition;
out vec2 vs_outUV;
out mat3 vs_outTBN;
out vec3 vs_outBarycentric;

// uniforms...
uniform mat4 matWorld;
uniform mat4 matView;
uniform mat4 matProj;
uniform mat4 matWorldInvTranspose;

void main()
{
	mat4 WVP = matProj * matView * matWorld;
	
	// position in world space
	vs_outPosition = (matWorld * vec4(in_Position, 1)).xyz;

	vs_outBarycentric = in_Barycentric;
	
	// pass on UVs
	vs_outUV = in_UV;

	// Create TBN matrix by transforming each vector into World Space
	vec3 T = normalize(vec3(matWorld * vec4(in_Tangent, 0)));
	vec3 B = normalize(vec3(matWorld * vec4(in_BiNormal, 0)));
	vec3 N = normalize(vec3(matWorld * vec4(in_Normal, 0)));

	// pass this matrix to fragment shader. This will be used to transform
	// vector from tangent space to world space.
	vs_outTBN = mat3(T,B,N);

	gl_Position = WVP * vec4(in_Position, 1.0);
}