
#include "PostProcess.h"
#include "LightsManager.h"
#include "Camera.h"
#include "PointLightObject.h"
#include "DirectionalLightObject.h"
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

	m_pPostFXShader = nullptr;
	m_pDeferredShader = nullptr;
	m_pBloomShader = nullptr;

	// properties
	m_fExposure = 1.0f;
	m_fGamma = 2.2f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
PostProcess::~PostProcess()
{
	if (m_pDeferredShader)
	{
		delete m_pDeferredShader;
		m_pDeferredShader = nullptr;
	}

	if (m_pBloomShader)
	{
		delete m_pBloomShader;
		m_pBloomShader = nullptr;
	}

	if (m_pPostFXShader)
	{
		delete m_pPostFXShader;
		m_pPostFXShader = nullptr;
	}

	glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_vao);

	glDeleteBuffers(1, &m_colorBuffer);
	glDeleteBuffers(1, &m_PositionBuffer);
	glDeleteBuffers(1, &m_AlbedoBuffer);
	glDeleteBuffers(1, &m_NormalBuffer);
	glDeleteBuffers(1, &m_DepthBuffer);
	glDeleteBuffers(1, &m_ShadowDepthBuffer);

	glDeleteFramebuffers(1, &m_fboDeferred);
	glDeleteRenderbuffers(1, &m_rboDeferred);

	glDeleteFramebuffers(1, &m_fboShadow);
	glDeleteRenderbuffers(1, &m_rboShadow);

	glDeleteFramebuffers(1, &m_fboBloom);
	glDeleteRenderbuffers(1, &m_rboBloom);
	
	glDeleteFramebuffers(1, &m_fboPostProcess);
	glDeleteRenderbuffers(1, &m_rboPostProcess);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PostProcess::SetupScreenQuad()
{
	// Create shader & grab handles to all the uniform variables...
	m_pDeferredShader = new GLSLShader("Shaders/vsDeferredLighting.glsl", "Shaders/psDeferredLighting.glsl");
	m_pBloomShader = new GLSLShader("Shaders/vsBloom.glsl", "Shaders/psBloom.glsl");
	m_pPostFXShader = new GLSLShader("Shaders/vsPostProcess.glsl", "Shaders/psPostProcess.glsl");

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
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(VertexPT), (void*)0);

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

	// ! EMISSION BUFFER
	glGenTextures(1, &m_EmissionBuffer);
	glBindTexture(GL_TEXTURE_2D, m_EmissionBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, horizRes, vertRes, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_EmissionBuffer, 0);

	// create render buffer object 
	glGenRenderbuffers(1, &m_rboDeferred);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rboDeferred);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, horizRes, vertRes);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	
	// attach render buffer to the framebuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rboDeferred);

	// Tell OGL which color attachments we will use
	GLuint attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, attachments);

	// Check if correct FBO was created or not ...
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PostProcess::CreateShadowMappingBuffers(int horizRes, int vertRes)
{
	// create framebuffer object
	glGenFramebuffers(1, &m_fboShadow);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboShadow);

	// Create color texture attachment for this fbo
	glGenTextures(1, &m_ShadowDepthBuffer);
	glBindTexture(GL_TEXTURE_2D, m_ShadowDepthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	// attach depth texture buffer to the framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_ShadowDepthBuffer, 0);

	glDrawBuffer(GL_NONE);

	// Check if correct FBO was created or not ...
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PostProcess::CreateBloomBuffers(int horizRes, int vertRes)
{
	// create framebuffer object
	glGenFramebuffers(1, &m_fboBloom);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboBloom);

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
	glGenRenderbuffers(1, &m_rboBloom);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rboBloom);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, horizRes, vertRes);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach render buffer to the framebuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rboBloom);

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
void PostProcess::BeginBloomPrepass()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboBloom);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PostProcess::EndBloomPrepass()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PostProcess::BeginShadowPass()
{
	glViewport(0, 0, 1024, 1024);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboShadow);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// To avoid peter-panning, only consider depth from back face instead of front face!
	glCullFace(GL_FRONT);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PostProcess::EndShadowPass()
{
	glCullFace(GL_BACK);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1280, 800);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PostProcess::ExecuteDeferredRenderPass()
{
	glDisable(GL_DEPTH_TEST);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	m_pDeferredShader->Use();
	int id = m_pDeferredShader->GetShaderID();

	SetDeferredPassShaderVariables(id);

	// bind all textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_PositionBuffer);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_NormalBuffer);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_AlbedoBuffer);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_EmissionBuffer);

	// Shadow Depth map
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, m_ShadowDepthBuffer);

	// Bind skybox texture
	glActiveTexture(GL_TEXTURE6);
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
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE6);
	GLSkybox::getInstance().UnbindCubemap();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PostProcess::ExecutePostprocessPass()
{
	glDisable(GL_DEPTH_TEST);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	m_pPostFXShader->Use();
	int id = m_pPostFXShader->GetShaderID();

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

	m_pBloomShader->Use();
	int id = m_pBloomShader->GetShaderID();

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
void PostProcess::DirectionalLightIlluminance(int shaderID)
{
	unsigned int numDirectionalLights = LightsManager::getInstance()->GetDirectionalLightsCount();
	glUniform1i(glGetUniformLocation(shaderID, "numDirectionalLights"), numDirectionalLights);

	for (GLuint i = 0; i < numDirectionalLights; ++i)
	{
		DirectionalLightObject* light = LightsManager::getInstance()->GetDirectionalLight(i);

		glm::vec3 direction = light->GetLightDirection();
		glm::vec4 color = light->GetLightColor();
		float intensity = light->GetLightIntensity();

		// form a string out of directional light IDs
		std::string dirLightDirectionStr = "dirLights[" + std::to_string(i) + "].direction";
		std::string dirLightColStr		 = "dirLights[" + std::to_string(i) + "].color";
		std::string dirLightIntensity	 = "dirLights[" + std::to_string(i) + "].intensity";

		glUniform3fv(glGetUniformLocation(shaderID, dirLightDirectionStr.c_str()), 1, glm::value_ptr(direction));
		glUniform4fv(glGetUniformLocation(shaderID, dirLightColStr.c_str()), 1, glm::value_ptr(color));
		glUniform1f(glGetUniformLocation(shaderID, dirLightIntensity.c_str()), intensity);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PostProcess::SetDeferredPassShaderVariables(int shaderID)
{
	glm::vec3 camPosition = Camera::getInstance().getCameraPosition();
	glm::mat4 camViewMatrix = Camera::getInstance().getViewMatrix();
	glm::mat4 camProjMatrix = Camera::getInstance().getProjectionMatrix();

	glm::mat4 worldToLightViewMatrix = LightsManager::getInstance()->GetDirectionalLight(0)->GetWorldToLightViewMatrix();
	glm::mat4 lightViewToProjectionMatrix = LightsManager::getInstance()->GetDirectionalLight(0)->GetLightViewToProjectionMatrix();

	// set camera position uniform variable
	GLuint hCamPosition = glGetUniformLocation(shaderID, "cameraPosition");
	GLuint hCameraViewMat = glGetUniformLocation(shaderID, "cameraViewMat");
	GLuint hCameraProjMat = glGetUniformLocation(shaderID, "cameraProjMat");

	// Set light related uniform variables
	GLuint hWorldToLightViewMat = glGetUniformLocation(shaderID, "matWorldToLigthView");
	GLuint hLightViewToProjMat = glGetUniformLocation(shaderID, "matLightViewToProjection");

	m_hPositionBuffer = glGetUniformLocation(shaderID, "positionBuffer");
	m_hNormalBuffer = glGetUniformLocation(shaderID, "normalBuffer");
	m_hAlbedoBuffer = glGetUniformLocation(shaderID, "albedoBuffer");
	m_hEmissiveBuffer = glGetUniformLocation(shaderID, "emissiveBuffer");
	
	m_hShadowDepthBuffer = glGetUniformLocation(shaderID, "shadowDepthBuffer");

	// Skybox reflection
	GLuint hCubeMap = glGetUniformLocation(shaderID, "texture_skybox");

	// Camera related uniforms
	glUniform3fv(hCamPosition, 1, glm::value_ptr(camPosition));
	glUniformMatrix4fv(hCameraViewMat, 1, GL_FALSE, glm::value_ptr(camViewMatrix));
	glUniformMatrix4fv(hCameraProjMat, 1, GL_FALSE, glm::value_ptr(camProjMatrix));

	// light/shadow related uniform
	glUniformMatrix4fv(hWorldToLightViewMat, 1, GL_FALSE, glm::value_ptr(worldToLightViewMatrix));
	glUniformMatrix4fv(hLightViewToProjMat, 1, GL_FALSE, glm::value_ptr(lightViewToProjectionMatrix));

	glUniform1i(m_hPositionBuffer, 0);
	glUniform1i(m_hNormalBuffer, 1);
	glUniform1i(m_hAlbedoBuffer, 2);
	glUniform1i(m_hEmissiveBuffer, 3);
	glUniform1i(m_hShadowDepthBuffer, 5);
	glUniform1i(hCubeMap, 6);

	// Point Light related uniform variables!
	PointLightIlluminance(shaderID);

	// Directional Light related uniform variables!
	DirectionalLightIlluminance(shaderID);
}

