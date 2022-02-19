
#include "ScreenAlignedQuad.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
ScreenAlignedQuad::ScreenAlignedQuad()
{
	m_posAtrrib = -1;
	m_texAttrib = -1;
	m_vao = -1;
	m_vbo = -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
ScreenAlignedQuad::~ScreenAlignedQuad()
{
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenAlignedQuad::CreateScreenAlignedQuad(int id)
{
	switch (id)
	{
		case QuadDesc::MAIN:
		{
			m_vertices[0] = VertexPT(glm::vec3(-1, 1, 0), glm::vec2(0, 1));
			m_vertices[1] = VertexPT(glm::vec3(-1, -1, 0), glm::vec2(0, 0));
			m_vertices[2] = VertexPT(glm::vec3(1, -1, 0), glm::vec2(1, 0));
			m_vertices[3] = VertexPT(glm::vec3(-1, 1, 0), glm::vec2(0, 1));
			m_vertices[4] = VertexPT(glm::vec3(1, -1, 0), glm::vec2(1, 0));
			m_vertices[5] = VertexPT(glm::vec3(1, 1, 0), glm::vec2(1, 1));

			break;
		}

		case QuadDesc::POSITION:
		{
			m_vertices[0] = VertexPT(glm::vec3(-1,		 1,     0), glm::vec2(0, 1));
			m_vertices[1] = VertexPT(glm::vec3(-1,		 0.33f, 0), glm::vec2(0, 0));
			m_vertices[2] = VertexPT(glm::vec3( -0.33f,  0.33f,	0), glm::vec2(1, 0));
			m_vertices[3] = VertexPT(glm::vec3(-1,		 1,	    0), glm::vec2(0, 1));
			m_vertices[4] = VertexPT(glm::vec3( -0.33f,	 0.33f,	0), glm::vec2(1, 0));
			m_vertices[5] = VertexPT(glm::vec3( -0.33f,  1,	    0), glm::vec2(1, 1));

			break;
		}

		case QuadDesc::NORMAL:
		{
			m_vertices[0] = VertexPT(glm::vec3(-0.33f,   1,     0), glm::vec2(0, 1));
			m_vertices[1] = VertexPT(glm::vec3(-0.33f,   0.33f, 0), glm::vec2(0, 0));
			m_vertices[2] = VertexPT(glm::vec3( 0.33f,   0.33f, 0), glm::vec2(1, 0));
			m_vertices[3] = VertexPT(glm::vec3(-0.33f,   1,     0), glm::vec2(0, 1));
			m_vertices[4] = VertexPT(glm::vec3( 0.33f,   0.33f, 0), glm::vec2(1, 0));
			m_vertices[5] = VertexPT(glm::vec3( 0.33f,   1,     0), glm::vec2(1, 1));

			break;
		}

		case QuadDesc::ALBEDO:
		{
			m_vertices[0] = VertexPT(glm::vec3( 0.33f, 1.0f,  0), glm::vec2(0, 1));
			m_vertices[1] = VertexPT(glm::vec3( 0.33f, 0.33f, 0), glm::vec2(0, 0));
			m_vertices[2] = VertexPT(glm::vec3( 1.0f,  0.33f, 0), glm::vec2(1, 0));
			m_vertices[3] = VertexPT(glm::vec3( 0.33f, 1.0f,  0), glm::vec2(0, 1));
			m_vertices[4] = VertexPT(glm::vec3( 1.0f,  0.33f, 0), glm::vec2(1, 0));
			m_vertices[5] = VertexPT(glm::vec3( 1.0f,  1.0f,  0), glm::vec2(1, 1));

			break;
		}

		case QuadDesc::EMISSION:
		{
			m_vertices[0] = VertexPT(glm::vec3(-1.0f,   0.33f,  0), glm::vec2(0, 1));
			m_vertices[1] = VertexPT(glm::vec3(-1.0f,  -0.33f,  0), glm::vec2(0, 0));
			m_vertices[2] = VertexPT(glm::vec3(-0.33f, -0.33f,  0), glm::vec2(1, 0));
			m_vertices[3] = VertexPT(glm::vec3(-1.0f,   0.33f,  0), glm::vec2(0, 1));
			m_vertices[4] = VertexPT(glm::vec3(-0.33f, -0.33f,  0), glm::vec2(1, 0));
			m_vertices[5] = VertexPT(glm::vec3(-0.33f,  0.33f,  0), glm::vec2(1, 1));

			break;
		}

		case QuadDesc::BRIGHTNESS:
		{
			m_vertices[0] = VertexPT(glm::vec3(-0.33f,  0.33f, 0), glm::vec2(0, 1));
			m_vertices[1] = VertexPT(glm::vec3(-0.33f, -0.33f, 0), glm::vec2(0, 0));
			m_vertices[2] = VertexPT(glm::vec3( 0.33f, -0.33f, 0), glm::vec2(1, 0));
			m_vertices[3] = VertexPT(glm::vec3(-0.33f,  0.33f, 0), glm::vec2(0, 1));
			m_vertices[4] = VertexPT(glm::vec3( 0.33f, -0.33f, 0), glm::vec2(1, 0));
			m_vertices[5] = VertexPT(glm::vec3( 0.33f,  0.33f, 0), glm::vec2(1, 1));

			break;
		}

		case QuadDesc::SHADOW_DEPTH:
		{
			m_vertices[0] = VertexPT(glm::vec3( 0.33f,  0.33f, 0), glm::vec2(0, 1));
			m_vertices[1] = VertexPT(glm::vec3( 0.33f, -0.33f, 0), glm::vec2(0, 0));
			m_vertices[2] = VertexPT(glm::vec3( 1.0f,  -0.33f, 0), glm::vec2(1, 0));
			m_vertices[3] = VertexPT(glm::vec3( 0.33f,  0.33f, 0), glm::vec2(0, 1));
			m_vertices[4] = VertexPT(glm::vec3( 1.0f,  -0.33f, 0), glm::vec2(1, 0));
			m_vertices[5] = VertexPT(glm::vec3( 1.0f,   0.33f, 0), glm::vec2(1, 1));

			break;
		}

		case QuadDesc::MASK:
		{
			m_vertices[0] = VertexPT(glm::vec3(-1.0f,  -0.33f, 0), glm::vec2(0, 1));
			m_vertices[1] = VertexPT(glm::vec3(-1.0f,  -1.0f,  0), glm::vec2(0, 0));
			m_vertices[2] = VertexPT(glm::vec3(-0.33f, -1.0f,  0), glm::vec2(1, 0));
			m_vertices[3] = VertexPT(glm::vec3(-1.0f,  -0.33f, 0), glm::vec2(0, 1));
			m_vertices[4] = VertexPT(glm::vec3(-0.33f, -1.0f,  0), glm::vec2(1, 0));
			m_vertices[5] = VertexPT(glm::vec3(-0.33f, -0.33f, 0), glm::vec2(1, 1));

			break;
		}

		default:
			break;
	}
	
	

	// create vao
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	
	// create vbo
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(VertexPT), m_vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(VertexPT), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(VertexPT), (void*)offsetof(VertexPT, uv));
	glBindVertexArray(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenAlignedQuad::RenderToScreenAlignedQuad()
{
	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

