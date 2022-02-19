#pragma once

#include "GL/glew.h"
#include "VertexStructures.h"

enum QuadDesc
{
	MAIN,
	POSITION,
	NORMAL,
	ALBEDO,
	EMISSION,
	BRIGHTNESS,
	SHADOW_DEPTH
};

class ScreenAlignedQuad
{
public:
	ScreenAlignedQuad();
	~ScreenAlignedQuad();

	void		CreateScreenAlignedQuad(int id);
	void		RenderToScreenAlignedQuad();

private:
	ScreenAlignedQuad(const ScreenAlignedQuad&);
	void operator=(const ScreenAlignedQuad&);

	GLuint		m_vbo;
	GLuint		m_vao;
	GLint		m_posAtrrib;
	GLint		m_texAttrib;
	VertexPT	m_vertices[6];
};