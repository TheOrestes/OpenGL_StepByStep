
#pragma once

#include "VertexStructures.h"

class GLSLShader;

class GLSkybox
{
public:
	static GLSkybox& getInstance()
	{
		static GLSkybox instance;
		return instance;
	}

	~GLSkybox();

	void		Init();
	void		Render();
	void		Update(float dt);
	void		Kill();

	void		BindCubemap();
	void		UnbindCubemap();

private:
	GLSkybox();

	GLuint		vao;
	GLuint		vbo;
	GLuint		ibo;
	GLuint		tbo;

	GLint		posAttrib;

	VertexP		vertices[8];
	GLuint		indices[36];

	GLint		hWorld;
	GLint		hView;
	GLint		hProj;

	glm::vec3	vecPosition;

	GLSLShader*	m_pShader;

	glm::mat4	matWorld;
	glm::mat4	matView;
	glm::mat4   matProj;
};