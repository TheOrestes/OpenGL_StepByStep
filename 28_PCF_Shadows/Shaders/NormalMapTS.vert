
#version 400

layout(location=0) in vec3 in_Position;
layout(location=1) in vec2 in_UV;
layout(location=2) in vec3 in_Normal;
layout(location=3) in vec3 in_BiNormal;
layout(location=4) in vec3 in_Tangent;

out vec3 vs_outPosition;
out vec2 vs_outUV;
out mat3 vs_outTBN;

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
	
	// pass on UVs
	vs_outUV = in_UV;

	// Create TBN matrix by transforming each vector into World Space
	vec3 T = normalize(vec3(matWorld * vec4(in_Tangent, 0)));
	vec3 B = normalize(vec3(matWorld * vec4(in_BiNormal, 0)));
	vec3 N = normalize(vec3(matWorld * vec4(in_Normal, 0)));

	// pass "transpose" of matrix to fragment shader, this matrix can be used 
	// to tranform any vector from world space to tangent space. 
	// Transpose is used instead of "Inverse" because TBN matrix is composed of
	// mutually perpendicular vectors, such matrix is called as orthogonal matrix.
	// Inverse of orthogonal matrix is simply transpose of that matrix!
	vs_outTBN = transpose(mat3(T,B,N));

	gl_Position = WVP * vec4(in_Position, 1.0);
}