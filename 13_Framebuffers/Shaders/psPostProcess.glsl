
#version 400

in vec2 vs_outTexcoord;
out vec4 outColor;

uniform sampler2D colorBuffer;

const float offset = 1.0 / 300.0;

vec2 offsets[9] = vec2[](
	vec2(-offset, offset),			// top-left
	vec2(0.0f, 	  offset),			// top-center
	vec2(offset,  offset),			// top-right
	vec2(-offset, 0.0f),			// center-left
	vec2(0.0f,    0.0f),			// center-center
	vec2(offset,  0.0f),			// center-right
	vec2(-offset, -offset),			// bottom-left
	vec2(0.0f,    -offset),			// bottom-center
	vec2(offset,  -offset)			// bottom-right
);

// Color inversion
vec4 invertColor(vec4 inColor)
{
	return vec4(vec4(1.0) - inColor);
}

// Grayscale
vec4 grayscaleColor(vec4 inColor)
{
	float avg = 0.2126 * inColor.r + 0.7152 * inColor.g + 0.0722 * inColor.b;
	return vec4(avg, avg, avg, 1.0);
}

vec4 sharpenKernel()
{
	// sharpen kernel...
	float kernel[9] = float[](
		-1, -1, -1,
		-1,  9, -1,
		-1, -1, -1
	);

	// Fetch neighbouring pixels
	vec3 sampleTex[9];
	for(int i = 0 ; i < 9 ; i++)
	{
		sampleTex[i] = vec3(texture2D(colorBuffer, vs_outTexcoord + offsets[i]));
	}

	vec3 col = vec3(0);
	for(int i = 0 ; i < 9 ; i++)
	{
		col += sampleTex[i] * kernel[i];
	}

	return vec4(col,1);
}

vec4 blurKernel()
{
	// blur kernel...
	float kernel[9] = float[](
		1.0/16, 2.0/16, 1.0/16,
		2.0/16, 4.0/16, 2.0/16,
		1.0/16, 2.0/16, 1.0/16
	);

	// Fetch neighbouring pixels
	vec3 sampleTex[9];
	for(int i = 0 ; i < 9 ; i++)
	{
		sampleTex[i] = vec3(texture2D(colorBuffer, vs_outTexcoord + offsets[i]));
	}

	vec3 col = vec3(0);
	for(int i = 0 ; i < 9 ; i++)
	{
		col += sampleTex[i] * kernel[i];
	}

	return vec4(col,1);
}

vec4 edgeDetectionKernel()
{
	// edge detection kernel...
	float kernel[9] = float[](
		1, 1, 1,
		1,-8, 1,
		1, 1, 1
	);

	// Fetch neighbouring pixels
	vec3 sampleTex[9];
	for(int i = 0 ; i < 9 ; i++)
	{
		sampleTex[i] = vec3(texture2D(colorBuffer, vs_outTexcoord + offsets[i]));
	}

	vec3 col = vec3(0);
	for(int i = 0 ; i < 9 ; i++)
	{
		col += sampleTex[i] * kernel[i];
	}

	return vec4(col,1);
}

vec4 laplaceKernel()
{
	// edge detection kernel...
	float kernel[9] = float[](
		0, 1, 0,
		1,-4, 1,
		0, 1, 0 
	);

	// Fetch neighbouring pixels
	vec3 sampleTex[9];
	for(int i = 0 ; i < 9 ; i++)
	{
		sampleTex[i] = vec3(texture2D(colorBuffer, vs_outTexcoord + offsets[i]));
	}

	vec3 col = vec3(0);
	for(int i = 0 ; i < 9 ; i++)
	{
		col += sampleTex[i] * kernel[i];
	}

	return vec4(col,1);
}

// http://setosa.io/ev/image-kernels/
vec4 embossKernel()
{
	// edge detection kernel...
	float kernel[9] = float[](
		-2, -1, 0,
		-1, 1, 1,
		0, 1, 2 
	);

	// Fetch neighbouring pixels
	vec3 sampleTex[9];
	for(int i = 0 ; i < 9 ; i++)
	{
		sampleTex[i] = vec3(texture2D(colorBuffer, vs_outTexcoord + offsets[i]));
	}

	vec3 col = vec3(0);
	for(int i = 0 ; i < 9 ; i++)
	{
		col += sampleTex[i] * kernel[i];
	}

	return vec4(col,1);
}

void main()
{
	// Simple post processing...
	vec4 screenColor = texture2D(colorBuffer, vs_outTexcoord); 
	outColor = invertColor(screenColor);

	// Kernel based post processing...
	//outColor = embossKernel();
}