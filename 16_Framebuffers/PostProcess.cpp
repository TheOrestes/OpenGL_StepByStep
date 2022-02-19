
#include "PostProcess.h"
#include "GLSLShader.h"
#include <iostream>

////////////////////////////////////////////////////////////////////////////////////////////////////
PostProcess::PostProcess()
{
	m_vbo = -1;
	m_vao = -1;
	m_colorBuffer = -1;
	m_hColorBuffer = -1;
	m_bWireframe = false;

	m_pShader = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
PostProcess::~PostProcess()
{
	if (m_pShader)
	{
		delete m_pShader;
		m_pShader = nullptr;
	}

	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_colorBuffer);
	glDeleteVertexArrays(1, &m_vao);
	glDeleteFramebuffers(1, &m_fbo);
	glDeleteRenderbuffers(1, &m_rbo);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PostProcess::SetupScreenQuad()
{
	// Create shader & grab handles to all the uniform variables...
	m_pShader = new GLSLShader("Shaders/vsPostProcess.glsl", "Shaders/psPostProcess.glsl");

	// Create screen aligned quad in NDC space

	m_vertices[0] = VertexPT(glm::vec3(-1, 1, 0), glm::vec2(0, 1));
	m_vertices[1] = VertexPT(glm::vec3(-1, -1, 0), glm::vec2(0, 0));
	m_vertices[2] = VertexPT(glm::vec3(1, -1, 0), glm::vec2(1, 0));
	m_vertices[3] = VertexPT(glm::vec3(-1, 1, 0), glm::vec2(0, 1));
	m_vertices[4] = VertexPT(glm::vec3(1, -1, 0), glm::vec2(1, 0));
	m_vertices[5] = VertexPT(glm::vec3(1, 1, 0), glm::vec2(1, 1));


	// create vao
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// create vbo
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(VertexPT), m_vertices, GL_STATIC_DRAW);
	
	GLuint shader = m_pShader->GetShaderID();
	//m_posAttrib = glGetAttribLocation(shader, "in_Position");
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(VertexPT), (void*)0);

	//m_texAttrib = glGetAttribLocation(shader, "in_Texcoord");
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(VertexPT), (void*)offsetof(VertexPT, uv));
	glBindVertexArray(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PostProcess::CreateColorDepthStencilBuffer(int horizRes, int vertRes)
{
	// create framebuffer object
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	// Create color texture attachment for this fbo
	glGenTextures(1, &m_colorBuffer);
	glBindTexture(GL_TEXTURE_2D, m_colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, horizRes, vertRes, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	// attach color texture buffer to the framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorBuffer, 0);

	// create render buffer object 
	glGenRenderbuffers(1, &m_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, horizRes, vertRes);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach render buffer to the framebuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

	// Tell OGL which color attachments we will use
	GLuint attachments[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachments);

	// Check if correct FBO was created or not ...
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PostProcess::BeginRTPass()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PostProcess::EndRTPass()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PostProcess::ExecutePostprocessPass()
{
	glDisable(GL_DEPTH_TEST);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	m_pShader->Use();
	int id = m_pShader->GetShaderID();

	m_hColorBuffer = glGetUniformLocation(id, "colorBuffer");
	glUniform1i(m_hColorBuffer, 0);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_colorBuffer);

	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PostProcess::SetWireframe(bool flag)
{
	m_bWireframe = flag;
}

