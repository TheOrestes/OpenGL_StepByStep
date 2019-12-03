
#include "GLSkybox.h"
#include "GLSLShader.h"
#include "Camera.h"
#include "TextureManager.h"

//////////////////////////////////////////////////////////////////////////////////////////
GLSkybox::GLSkybox()
{
	// initialize skybox vertices...
	vertices[0] = VertexP(glm::vec3(-1,-1,1));
	vertices[1] = VertexP(glm::vec3(1,-1,1));
	vertices[2] = VertexP(glm::vec3(1,1,1));
	vertices[3] = VertexP(glm::vec3(-1,1,1));
	vertices[4] = VertexP(glm::vec3(-1,-1,-1));
	vertices[5] = VertexP(glm::vec3(1,-1,-1));
	vertices[6] = VertexP(glm::vec3(1,1,-1));
	vertices[7] = VertexP(glm::vec3(-1,1,-1));

	// initialize skybox indices...
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

	// set position to origin
	vecPosition = glm::vec3(0,0,0);
}

//////////////////////////////////////////////////////////////////////////////////////////
GLSkybox::~GLSkybox()
{
	Kill();
}

//////////////////////////////////////////////////////////////////////////////////////////
void GLSkybox::Init()
{
	// create skybox instance...
	m_pShader = new GLSLShader("Shaders/Skybox.vert", "Shaders/SkyboxDeferred.frag");

	// Load cubemap and assign id ...
	tbo = TextureManager::getInstannce().LoadCubemapFromFile("../Assets/cubemaps/Yokohama2");

	// create vao
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);

	glBindVertexArray(vao);

		// create vbo
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(VertexP), vertices, GL_STATIC_DRAW);

		// create ibo
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		GLuint shader = m_pShader->GetShaderID();

		//posAttrib = glGetAttribLocation(shader, "in_Position");
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexP), (void*)0);

	glBindVertexArray(0);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GLSkybox::BindCubemap()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, tbo);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GLSkybox::UnbindCubemap()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GLSkybox::Render()
{
	// So based on shader for skybox, we have made sure that third component equal to w, 
	// perspective divide always returns 1. This way depth value is always 1 which is at
    // back of all the geometries rendered. 
	// The depth buffer will be filled with values of 1.0 for the skybox, so we need to 
	// make sure the skybox passes the depth tests with values less than or equal to the 
	// depth buffer instead of less than. 
	glDepthFunc(GL_LEQUAL);
	m_pShader->Use();

	GLuint shader = m_pShader->GetShaderID();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tbo);

	glUniformMatrix4fv(glGetUniformLocation(shader, "matWorld"), 1, GL_FALSE, glm::value_ptr(matWorld));
	glUniformMatrix4fv(glGetUniformLocation(shader, "matView"), 1, GL_FALSE, glm::value_ptr(matView));
	glUniformMatrix4fv(glGetUniformLocation(shader, "matProj"), 1, GL_FALSE, glm::value_ptr(matProj));

	glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);	

	// Set depth function back to default
	glDepthFunc(GL_LESS);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	
}

//////////////////////////////////////////////////////////////////////////////////////////
void GLSkybox::Update( float dt )
{
	matWorld = glm::translate(glm::mat4(1), vecPosition);

	// Note that, we first convert matrix to 3x3 so that we remove the translation part, later
	// we convert back to 4x4 matrix keeping the rotation part intact! 
	// All this is done so that, when player moves, skybox moves with him giving impression that
	// skybox is far far away!
	matView = glm::mat4(glm::mat3(Camera::getInstance().getViewMatrix()));

	matProj = Camera::getInstance().getProjectionMatrix();
}

//////////////////////////////////////////////////////////////////////////////////////////
void GLSkybox::Kill()
{
	delete m_pShader;

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
	glDeleteBuffers(1, &tbo);
	glDeleteVertexArrays(1, &vao);
}



