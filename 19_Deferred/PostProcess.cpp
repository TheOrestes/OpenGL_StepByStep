
#include "PostProcess.h"
#include "LightsManager.h"
#include "Camera.h"
#include "PointLightObject.h"
#include "GLSkybox.h"
#include "GLSLShader.h"
#include <iostream>

////////////////////////////////////////////////////////////////////////////////////////////////////
PostProcess::PostProcess()
{
	// Screen aligned quad
	m_vbo = -1;
	m_vao = -1;

	// Postprocess
	m_colorBuffer = -1;
	m_hColorBuffer = -1;

	// Deferred
	m_PositionBuffer = -1;
	m_NormalBuffer = -1;
	m_AlbedoBuffer = -1;
	m_hPositionBuffer = -1;
	m_hNormalBuffer = -1;
	m_hAlbedoBuffer = -1;

	m_bWireframe = false;

	m_pShader = nullptr;

	// properties
	m_fExposure = 1.0f;
	m_fGamma = 2.2f;
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
	glDeleteVertexArrays(1, &m_vao);

	glDeleteBuffers(1, &m_colorBuffer);
	glDeleteBuffers(1, &m_PositionBuffer);
	glDeleteBuffers(1, &m_AlbedoBuffer);
	glDeleteBuffers(1, &m_NormalBuffer);

	glDeleteFramebuffers(1, &m_fboDeferred);
	glDeleteRenderbuffers(1, &m_rboDeferred);
	
	glDeleteFramebuffers(1, &m_fboPostProcess);
	glDeleteRenderbuffers(1, &m_rboPostProcess);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PostProcess::SetupScreenQuad()
{
	// Create shader & grab handles to all the uniform variables...
	m_pShader = new GLSLShader("Shaders/vsDeferredLighting.glsl", "Shaders/psDeferredLighting.glsl");

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
	glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(VertexPT), (void*)offsetof(VertexPT,
																						uv));
	glBindVertexArray(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PostProcess::CreateDeferredBuffers(int horizRes, int vertRes)
{
	// Create framebuffer object
	glGenFramebuffers(1, &m_fboDeferred);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboDeferred);

	// ! POSITION BUFFER
	glGenTextures(1, &m_PositionBuffer);
	glBindTexture(GL_TEXTURE_2D, m_PositionBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, horizRes, vertRes, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_PositionBuffer, 0);

	// ! NORMAL(RGB) + SPECULAR(A) BUFFER
	glGenTextures(1, &m_NormalBuffer);
	glBindTexture(GL_TEXTURE_2D, m_NormalBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, horizRes, vertRes, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_NormalBuffer, 0);

	// ! ALBEDO BUFFER
	glGenTextures(1, &m_AlbedoBuffer);
	glBindTexture(GL_TEXTURE_2D, m_AlbedoBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, horizRes, vertRes, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_AlbedoBuffer, 0);

	// create render buffer object 
	glGenRenderbuffers(1, &m_rboDeferred);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rboDeferred);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, horizRes, vertRes);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach render buffer to the framebuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rboDeferred);

	// Tell OGL which color attachments we will use
	GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	// Check if correct FBO was created or not ...
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PostProcess::CreateColorDepthStencilBuffer(int horizRes, int vertRes)
{
	// create framebuffer object
	glGenFramebuffers(1, &m_fboPostProcess);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboPostProcess);

	// Create color texture attachment for this fbo
	glGenTextures(1, &m_colorBuffer);
	glBindTexture(GL_TEXTURE_2D, m_colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, horizRes, vertRes, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	// attach color texture buffer to the framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorBuffer, 0);

	// create render buffer object 
	glGenRenderbuffers(1, &m_rboPostProcess);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rboPostProcess);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, horizRes, vertRes);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach render buffer to the framebuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rboPostProcess);

	// Tell OGL which color attachments we will use
	GLuint attachments[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachments);

	// Check if correct FBO was created or not ...
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PostProcess::CreateColorBrightnessBuffer(int horizRes, int vertRes)
{
	// create framebuffer object
	glGenFramebuffers(1, &m_fboPostProcess);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboPostProcess);

	// ! COLOR BUFFER
	// Create color texture attachment for this fbo
	glGenTextures(1, &m_colorBuffer);
	glBindTexture(GL_TEXTURE_2D, m_colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, horizRes, vertRes, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	// attach color texture buffer to the framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorBuffer, 0);

	// ! BRIGHTNESS BUFFER
	// Create brightness texture attachment for this fbo
	glGenTextures(1, &m_brightBuffer);
	glBindTexture(GL_TEXTURE_2D, m_brightBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, horizRes, vertRes, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	// attach bright texture buffer to the framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_brightBuffer, 0);

	// create render buffer object 
	glGenRenderbuffers(1, &m_rboPostProcess);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rboPostProcess);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, horizRes, vertRes);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach render buffer to the framebuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rboPostProcess);

	// Tell OGL which color attachments we will use
	GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	// Check if correct FBO was created or not ...
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PostProcess::BeginRTPass()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboPostProcess);
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
void PostProcess::BeginGeometryRenderPass()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboDeferred);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PostProcess::EndGeometryRenderPass()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PostProcess::ExecuteDeferredRenderPass()
{
	glDisable(GL_DEPTH_TEST);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	m_pShader->Use();
	int id = m_pShader->GetShaderID();

	SetDeferredPassShaderVariables(id);

	// bind all textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_PositionBuffer);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_NormalBuffer);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_AlbedoBuffer);

	// Bind skybox texture
	glActiveTexture(GL_TEXTURE3);
	GLSkybox::getInstance().BindCubemap();

	// Render Quad!
	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	// Unbind all textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE3);
	GLSkybox::getInstance().UnbindCubemap();
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

	GLint m_hGamma = glGetUniformLocation(id, "gamma");
	glUniform1f(m_hGamma, m_fGamma);

	GLint m_hExposure = glGetUniformLocation(id, "exposure");
	glUniform1f(m_hExposure, m_fExposure);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_colorBuffer);

	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PostProcess::ExecuteBloomPass()
{
	glDisable(GL_DEPTH_TEST);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	m_pShader->Use();
	int id = m_pShader->GetShaderID();

	m_hColorBuffer = glGetUniformLocation(id, "colorBuffer");
	glUniform1i(m_hColorBuffer, 0);

	m_hBrightBuffer = glGetUniformLocation(id, "brightBuffer");
	glUniform1i(m_hBrightBuffer, 1);

	GLint m_hGamma = glGetUniformLocation(id, "gamma");
	glUniform1f(m_hGamma, m_fGamma);

	GLint m_hExposure = glGetUniformLocation(id, "exposure");
	glUniform1f(m_hExposure, m_fExposure);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_colorBuffer);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_brightBuffer);
	glGenerateMipmap(GL_TEXTURE_2D);

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

////////////////////////////////////////////////////////////////////////////////////////////////////
void PostProcess::PointLightIlluminance(int shaderID)
{
	unsigned int numPointLights = LightsManager::getInstance()->GetPointlightsCount();
	glUniform1i(glGetUniformLocation(shaderID, "numPointLights"), numPointLights);

	for (GLuint i = 0; i < numPointLights; ++i)
	{
		PointLightObject* light = LightsManager::getInstance()->GetPointLight(i);

		glm::vec3 position = light->GetLightPosition();
		glm::vec4 color = light->GetLightColor();
		float intensity = light->GetLightIntensity();
		float radius = light->GetLightRadius();

		// form a string out of point light Ids
		std::string pointLightPosStr = "pointLights[" + std::to_string(i) + "].position";
		std::string pointLightColStr = "pointLights[" + std::to_string(i) + "].color";
		std::string pointLightIntStr = "pointLights[" + std::to_string(i) + "].intensity";
		std::string pointLightRadStr = "pointLights[" + std::to_string(i) + "].radius";

		glUniform3fv(glGetUniformLocation(shaderID, pointLightPosStr.c_str()), 1, glm::value_ptr(position));
		glUniform4fv(glGetUniformLocation(shaderID, pointLightColStr.c_str()), 1, glm::value_ptr(color));
		glUniform1f(glGetUniformLocation(shaderID, pointLightIntStr.c_str()), intensity);
		glUniform1f(glGetUniformLocation(shaderID, pointLightRadStr.c_str()), radius);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PostProcess::SetDeferredPassShaderVariables(int shaderID)
{
	glm::vec3 camPosition = Camera::getInstance().getCameraPosition();

	// set camera position uniform variable
	GLuint hCamPosition = glGetUniformLocation(shaderID, "cameraPosition");
	m_hPositionBuffer = glGetUniformLocation(shaderID, "positionBuffer");
	m_hNormalBuffer = glGetUniformLocation(shaderID, "normalBuffer");
	m_hAlbedoBuffer = glGetUniformLocation(shaderID, "albedoBuffer");

	// Skybox reflection
	GLuint hCubeMap = glGetUniformLocation(shaderID, "texture_skybox");

	glUniform3fv(hCamPosition, 1, glm::value_ptr(camPosition));
	glUniform1i(m_hPositionBuffer, 0);
	glUniform1i(m_hNormalBuffer, 1);
	glUniform1i(m_hAlbedoBuffer, 2);
	glUniform1i(hCubeMap, 3);

	// Point Light related uniform variables!
	PointLightIlluminance(shaderID);
}

