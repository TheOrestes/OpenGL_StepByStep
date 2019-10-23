#include "GLQuad.h"
#include "GLSLShader.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "TextureManager.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
GLQuad::GLQuad()
{
	// vertex data
	//
	//  1--2		(0,0)----(1,0)
	//  | /|		  |		   |
	//	|/ |		  |        |
	//	0--3		(0,1)----(1,1)

	vertices[0] = VertexPT(glm::vec3(0, 0, 0), glm::vec2(0, 1));
	vertices[1] = VertexPT(glm::vec3(0, 1, 0), glm::vec2(0, 0));
	vertices[2] = VertexPT(glm::vec3(1, 1, 0), glm::vec2(1, 0));
	vertices[3] = VertexPT(glm::vec3(1, 0, 0), glm::vec2(1, 1));

	// Index data
	//
	//  1--2		4
	//  | /		   /|
	//	|/		  /	|
	//	0		 3--5

	indices[0] = 0;			indices[1] = 1;			indices[2] = 2;
	indices[3] = 0;			indices[4] = 2;			indices[5] = 3;

	// Set default position & scale
	SetPosition(glm::vec3(0));
	SetScale(glm::vec3(2));
	SetRotation(glm::vec3(0, 1, 0), 45.0f);

	// Wireframe?
	m_bWireframe = false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
GLQuad::~GLQuad()
{
	Kill();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void GLQuad::Init()
{
	// Create shader object
	m_pShader = new GLSLShader("Shaders/vsTexture.glsl", "Shaders/psTexture.glsl");

	// create vao
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// create vbo
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(VertexPT), vertices, GL_STATIC_DRAW);

	// create ibo
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// create tbo
	tbo = TextureManager::getInstannce().Load2DTextureFromFile("Randy.jpg", "W:/ICAT/Source/Rupesh/1_WindowCreation_Quad_Texture/Data");

	GLuint shader = m_pShader->GetShaderID();
	//posAttrib = glGetAttribLocation(shader, "in_Position");
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(VertexPT), (void*)0);

	//texAttrib = glGetAttribLocation(shader, "in_Texcoord");
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(VertexPT), (void*)offsetof(VertexPT, uv));

	SetupViewProjMatrix();

	//glEnableVertexAttribArray(0);
	glBindVertexArray(0); 


	glEnable(GL_DEPTH_TEST);
	
	// WIREFRAME MODE!
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void GLQuad::Update(float dt)
{
	static float angle = 0.0f;
	angle += dt;
	
	// create world transformation matrix
	// So what we are doing is, basically glm::translate/rotate etc matrices take first param
	// as an argument which usually is kept to be identity matrix. Internally, these functions,
	// multiply with the first argument.
	// https://www.youtube.com/watch?v=U_RtSchYYec

	glm::mat4 T   = glm::translate(glm::mat4(1), vecPosition);
	glm::mat4 TR  = glm::rotate(T, angle, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 TRS = glm::scale(TR, glm::vec3(vecScale.x, vecScale.y, vecScale.z));
	
	// Set final World transformation matrix...
	matWorld = TRS;

	// Create composite ViewProjection matrix
	//matWorld = glm::rotate(matWorld, 0.1f*dt, glm::vec3(0.0f, 1.0f, 0.0f));
	matView = glm::lookAt(glm::vec3(0,0,5), glm::vec3(0,0,0), glm::vec3(0,1,0));	
	matProj = glm::perspective(45.0f, 1.6f, 0.1f, 1000.0f);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void GLQuad::Render()
{
	glBindVertexArray(vao);

	m_pShader->Use();

	glUniform1i(glGetUniformLocation(m_pShader->GetShaderID(), "texture_diffuse"), 0);

	glUniformMatrix4fv(hWorld, 1, GL_FALSE, glm::value_ptr(matWorld));
	glUniformMatrix4fv(hView, 1, GL_FALSE, glm::value_ptr(matView));
	glUniformMatrix4fv(hProj, 1, GL_FALSE, glm::value_ptr(matProj));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tbo);

	// know more about the last element!
	// http://stackoverflow.com/questions/17191258/no-display-from-gldrawelements
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void GLQuad::Kill()
{
	delete m_pShader;

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
	glDeleteVertexArrays(1, &vao);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void GLQuad::SetPosition(const glm::vec3& loc)
{
	vecPosition = loc;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void GLQuad::SetScale(const glm::vec3& sc)
{
	vecScale = sc;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void GLQuad::SetRotation(const glm::vec3& ax, float angle)
{
	vecRotationAxis = ax;
	m_fAngle = angle;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void GLQuad::SetWireframe(bool flag)
{
	m_bWireframe = flag;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void GLQuad::SetupViewProjMatrix()
{
	GLuint shader = m_pShader->GetShaderID();

	hWorld = glGetUniformLocation(shader, "matWorld");
	hView = glGetUniformLocation(shader, "matView");
	hProj = glGetUniformLocation(shader, "matProj");
}
