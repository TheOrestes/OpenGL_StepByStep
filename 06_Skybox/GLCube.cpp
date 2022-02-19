
#include "GLCube.h"
#include "GLSLShader.h"
#include "VertexStructures.h"
#include "Camera.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

//////////////////////////////////////////////////////////////////////////////////////////
GLCube::GLCube()
{
	// Vertex Data
	vertices[0] = VertexPC(glm::vec3(-1,-1,1), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	vertices[1] = VertexPC(glm::vec3(1,-1,1), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	vertices[2] = VertexPC(glm::vec3(1,1,1), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	vertices[3] = VertexPC(glm::vec3(-1,1,1), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	vertices[4] = VertexPC(glm::vec3(-1,-1,-1), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	vertices[5] = VertexPC(glm::vec3(1,-1,-1), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	vertices[6] = VertexPC(glm::vec3(1,1,-1), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	vertices[7] = VertexPC(glm::vec3(-1,1,-1), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

	// Index data ( 6 faces = 12 triangles = 36 indices )

	indices[0] = 0;				indices[1] = 1;			indices[2] = 2;
	indices[3] = 2;				indices[4] = 3;			indices[5] = 0;

	indices[6] = 3;				indices[7] = 2;			indices[8] = 6;
	indices[9] = 6;				indices[10] = 7;		indices[11] = 3;

	indices[12] = 7;			indices[13] = 6;		indices[14] = 5;
	indices[15] = 5;			indices[16] = 4;		indices[17] = 7;

	indices[18] = 4;			indices[19] = 5;		indices[20] = 1;
	indices[21] = 1;			indices[22] = 0;		indices[23] = 4;

	indices[24] = 4;			indices[25] = 0;		indices[26] = 3;
	indices[27] = 3;			indices[28] = 7;		indices[29] = 4;

	indices[30] = 1;			indices[31] = 5;		indices[32] = 6;
	indices[33] = 6;			indices[34] = 2;		indices[35] = 1;

	// set default position & scale
	SetPosition(glm::vec3(0,0,0));
	SetScale(glm::vec3(1,1,1));
	SetRotation(glm::vec3(0,1,0), 45.0f);

	// Wireframe?
	m_bWireframe = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
GLCube::GLCube(const glm::vec4& color)
{
	// Vertex Data
	vertices[0] = VertexPC(glm::vec3(-1,-1,1), color);
	vertices[1] = VertexPC(glm::vec3(1,-1,1), color);
	vertices[2] = VertexPC(glm::vec3(1,1,1), color);
	vertices[3] = VertexPC(glm::vec3(-1,1,1), color);
	vertices[4] = VertexPC(glm::vec3(-1,-1,-1), color);
	vertices[5] = VertexPC(glm::vec3(1,-1,-1), color);
	vertices[6] = VertexPC(glm::vec3(1,1,-1), color);
	vertices[7] = VertexPC(glm::vec3(-1,1,-1), color);

	// Index data ( 6 faces = 12 triangles = 36 indices )

	indices[0] = 0;				indices[1] = 1;			indices[2] = 2;
	indices[3] = 2;				indices[4] = 3;			indices[5] = 0;

	indices[6] = 3;				indices[7] = 2;			indices[8] = 6;
	indices[9] = 6;				indices[10] = 7;		indices[11] = 3;

	indices[12] = 7;			indices[13] = 6;		indices[14] = 5;
	indices[15] = 5;			indices[16] = 4;		indices[17] = 7;

	indices[18] = 4;			indices[19] = 5;		indices[20] = 1;
	indices[21] = 1;			indices[22] = 0;		indices[23] = 4;

	indices[24] = 4;			indices[25] = 0;		indices[26] = 3;
	indices[27] = 3;			indices[28] = 7;		indices[29] = 4;

	indices[30] = 1;			indices[31] = 5;		indices[32] = 6;
	indices[33] = 6;			indices[34] = 2;		indices[35] = 1;

	// set default position & scale
	SetPosition(glm::vec3(0,0,0));
	SetScale(glm::vec3(1,1,1));
	SetRotation(glm::vec3(0,1,0), 45.0f);

	// Wireframe?
	m_bWireframe = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
GLCube::~GLCube()
{
	Kill();
}

//////////////////////////////////////////////////////////////////////////////////////////
void GLCube::SetupViewProjMatrix()
{
	GLuint shader = m_pShader->GetShaderID();

	hWorld = glGetUniformLocation(shader, "matWorld");
	hView = glGetUniformLocation(shader, "matView");
	hProj = glGetUniformLocation(shader, "matProj");
}

//////////////////////////////////////////////////////////////////////////////////////////
void GLCube::SetPosition(const glm::vec3& loc)
{
	vecPosition = loc;
}

//////////////////////////////////////////////////////////////////////////////////////////
void GLCube::SetScale(const glm::vec3& sc)
{
	vecScale = sc;
}

//////////////////////////////////////////////////////////////////////////////////////////
void GLCube::SetRotation( const glm::vec3& ax, float angle )
{
	vecRotationAxis = ax;
	m_fAngle = angle;
}

//////////////////////////////////////////////////////////////////////////////////////////
void GLCube::Init()
{
	// Create shader object
	// shader
	m_pShader = new GLSLShader("Shaders/vs.glsl", "Shaders/ps.glsl");

	// create vao
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// create vbo
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(VertexPC), vertices, GL_STATIC_DRAW);

	// create ibo
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	GLuint shader = m_pShader->GetShaderID();
	posAttrib = glGetAttribLocation(shader, "in_Position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, false, sizeof(VertexPC), (void*)0);

	colAttrib = glGetAttribLocation(shader, "in_Color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 4, GL_FLOAT, false, sizeof(VertexPC), (void*)offsetof(VertexPC, color));

	SetupViewProjMatrix();

	//glEnableVertexAttribArray(0);
	glBindVertexArray(0); 


	glEnable(GL_DEPTH_TEST);
	
	// WIREFRAME MODE!
	if(m_bWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
}

//////////////////////////////////////////////////////////////////////////////////////////
void GLCube::Update(float dt)
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
	matView = Camera::getInstance().getViewMatrix(); //glm::lookAt(glm::vec3(0,0,5), glm::vec3(0,0,0), glm::vec3(0,1,0));	
	matProj = Camera::getInstance().getProjectionMatrix(); //glm::perspective(45.0f, 1.6f, 0.1f, 1000.0f);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GLCube::Render()
{
	glBindVertexArray(vao);

	m_pShader->Use();

	glUniformMatrix4fv(hWorld, 1, GL_FALSE, glm::value_ptr(matWorld));
	glUniformMatrix4fv(hView, 1, GL_FALSE, glm::value_ptr(matView));
	glUniformMatrix4fv(hProj, 1, GL_FALSE, glm::value_ptr(matProj));

	// know more about the last element!
	// http://stackoverflow.com/questions/17191258/no-display-from-gldrawelements
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GLCube::Kill()
{
	delete m_pShader;

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
	glDeleteVertexArrays(1, &vao);
}


