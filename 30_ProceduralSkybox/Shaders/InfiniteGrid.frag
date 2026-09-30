
#version 400

// Unity/Unreal-style infinite editor grid. No mesh - this is a full-screen pass that reconstructs
// a world-space ray per pixel, intersects it with the world's y=0 plane, and draws anti-aliased
// grid lines there. Unlit (fixed color, independent of scene lighting) so it stays visible even
// when the sun is below the horizon and everything else has gone black - but still occluded
// correctly by opaque scene geometry using the G-buffer's position/sky data, 
// since this FBO's real depth attachment isn't populated with scene depth.

in vec2 vs_outTexcoord;
out vec4 outColor;

uniform mat4		matInvViewProj;
uniform vec3		cameraPosition;
uniform float		cellSize;
uniform float		fadeDistance;

uniform sampler2D	positionBuffer;
uniform sampler2D	objectIDBuffer;

void main()
{
	// Reconstruct the world-space point this pixel's ray hits at the far plane, then build a ray
	// from the camera through it - same inverse-view-projection technique used elsewhere for
	// going backwards from screen space to world space.
	vec4 clipPos = vec4(vs_outTexcoord * 2.0f - 1.0f, 1.0f, 1.0f);
	vec4 farWorld4 = matInvViewProj * clipPos;
	vec3 farWorld = farWorld4.xyz / farWorld4.w;
	vec3 rayDir = normalize(farWorld - cameraPosition);

	// Intersect with the ground plane (y = 0). Looking parallel to it, or away from it, means no
	// grid on this pixel.
	if (abs(rayDir.y) < 1e-4f)
		discard;

	float t = -cameraPosition.y / rayDir.y;
	if (t <= 0.0f)
		discard;

	vec3 worldPos = cameraPosition + rayDir * t;

	// Occlusion against real scene geometry: sky pixels (ObjectID.g == 1) write garbage position
	// data (see HDRISkybox.frag), so treat them as infinitely far rather than reading gPosition.
	vec3 objectID = texture(objectIDBuffer, vs_outTexcoord).rgb;
	float sceneDistance = 1e9f;
	if (objectID.g < 0.5f)
	{
		vec3 scenePos = texture(positionBuffer, vs_outTexcoord).rgb;
		sceneDistance = length(scenePos - cameraPosition);
	}
	if (t >= sceneDistance)
		discard;

	// Anti-aliased grid lines via screen-space derivatives (standard "infinite grid" technique) -
	// stays crisp at any distance/angle instead of aliasing like a naive fract()-based line test.
	vec2 coord = worldPos.xz / cellSize;
	vec2 minorDerivative = fwidth(coord);
	vec2 minorGrid = abs(fract(coord - 0.5f) - 0.5f) / max(minorDerivative, vec2(1e-6f));
	float minorLine = 1.0f - min(min(minorGrid.x, minorGrid.y), 1.0f);

	// Every 10th line drawn thicker/brighter ("major" line), same convention every 3D editor uses.
	vec2 majorCoord = coord / 10.0f;
	vec2 majorDerivative = fwidth(majorCoord);
	vec2 majorGrid = abs(fract(majorCoord - 0.5f) - 0.5f) / max(majorDerivative, vec2(1e-6f));
	float majorLine = 1.0f - min(min(majorGrid.x, majorGrid.y), 1.0f);

	// World axis lines through the origin: x == 0 (runs along Z) and z == 0 (runs along X).
	float axisLineWidth = max(fwidth(worldPos.x) + fwidth(worldPos.z), 1e-6f);
	float lineAtXZero = 1.0f - min(abs(worldPos.x) / axisLineWidth, 1.0f);
	float lineAtZZero = 1.0f - min(abs(worldPos.z) / axisLineWidth, 1.0f);
	float originAxis = max(lineAtXZero, lineAtZZero);

	// Neutral grays - minor lines dim, major (every-10th) lines noticeably brighter - plus a maroon
	// pair of lines through the origin, rather than a plain white grid.
	vec3 minorColor = vec3(0.3f) * 0.1f;
	vec3 majorColor = vec3(0.65f) * 0.1f;
	vec3 originColor = vec3(0.45f, 0.05f, 0.08f) * 0.1f;

	vec3 color = mix(minorColor, majorColor, majorLine);
	float coverage = max(minorLine, majorLine);

	color = mix(color, originColor, originAxis);
	coverage = max(coverage, originAxis);

	// Fade out smoothly with distance so the grid doesn't alias/pop near the horizon.
	coverage *= 1.0f - smoothstep(fadeDistance * 0.5f, fadeDistance, t);

	if (coverage <= 0.001f)
		discard;

	outColor = vec4(color, coverage);
}