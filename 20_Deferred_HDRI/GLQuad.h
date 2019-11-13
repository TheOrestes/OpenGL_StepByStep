#pragma once


#pragma once

#include "GL\glew.h"
#include "VertexStructures.h"

class GLSLShader;

class GLQuad
{
public:
	GLQuad();
	~GLQuad();

	void		Init();
	void		Update(float dt);
	void		Render();
	void		Kill();

	void		SetPosition(const glm::vec3& loc);
	void		SetScale(const glm::vec3& sc);
	void		SetRotation(const glm::vec3& ax, float angle);
	void		SetWireframe(bool flag);

private:
	GLQuad(const GLQuad&);
	void operator=(const GLQuad&);

	void		SetupViewProjMatrix();

	GLuint		vbo;				// vertex buffer object
	GLuint		ibo;				// index buffer object
	GLuint		tbo;				// texture object

	GLuint		vao;				// vertex array object

	GLint		posAttrib;			// vertex attribute id
	GLint		texAttrib;			// color attribute id
	VertexPT	vertices[4];		// vertices of a cube
	GLuint		indices[6];			// indices of a cube

	GLint		hWorld;				// handle to World matrix
	GLint		hView;				// handle to View matrix
	GLint		hProj;				// handle to projection matrix

	glm::vec3	vecPosition;		// World Space Position
	glm::vec3	vecScale;			// Scale of an object
	glm::vec3	vecRotationAxis;	// rotation axis
	float		m_fAngle;			// angle of rotation

	bool		m_bWireframe;		// wire-frame rendering flag

	GLSLShader*	m_pShader;			// Ptr to shader object

	glm::mat4   matWorld;
	glm::mat4	matView;
	glm::mat4	matProj;
};
