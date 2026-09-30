#pragma once

#include "GL/glew.h"

class GLSLShader;
class ScreenAlignedQuad;

// Unity/Unreal-style infinite editor grid on the world's XZ (y=0) plane. Not a mesh - a full-screen
// pass whose fragment shader reconstructs a world-space ray per pixel and intersects it with that
// plane, so the grid is genuinely unbounded with no LOD/tiling to manage. Drawn unlit and alpha-
// blended on top of the already-lit scene (see InfiniteGrid.frag), so it stays visible even when
// the sun is below the horizon and everything else has gone black, while still respecting occlusion
// against opaque scene geometry.
class InfiniteGrid
{
public:
	static InfiniteGrid& getInstance()
	{
		static InfiniteGrid instance;
		return instance;
	}

	~InfiniteGrid();

	void			Initialize();
	void			Render(GLuint positionBuffer, GLuint objectIDBuffer);

	inline bool		IsEnabled()				{ return m_bEnabled; }
	inline void		SetEnabled(bool enabled) { m_bEnabled = enabled; }

	inline float	GetCellSize()				{ return m_fCellSize; }
	inline void		SetCellSize(float size)		{ m_fCellSize = size; }

	inline float	GetFadeDistance()				{ return m_fFadeDistance; }
	inline void		SetFadeDistance(float dist)		{ m_fFadeDistance = dist; }

private:
	InfiniteGrid();
	InfiniteGrid(const InfiniteGrid&);
	void operator=(const InfiniteGrid&);

	bool				m_bEnabled;
	float				m_fCellSize;		// world units between minor grid lines
	float				m_fFadeDistance;	// distance at which the grid has fully faded out

	GLSLShader*			m_pShader;
	ScreenAlignedQuad*	m_pScreenQuad;
};
