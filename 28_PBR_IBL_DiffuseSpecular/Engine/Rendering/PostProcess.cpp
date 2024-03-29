
#include "PostProcess.h"
#include "GLSLShader.h"

#include "../Lighting/LightsManager.h"
#include "../Lighting/PointLightObject.h"
#include "../Lighting/DirectionalLightObject.h"
#include "../Gameobjects/Camera.h"
#include "../Gameobjects/GLSkybox.h"
#include "../Gameobjects/HDRSkybox.h"
#include "../Rendering/ScreenAlignedQuad.h"
#include "../Main/Helper.h"
#include "../UI/UIManager.h"
#include "../Main/Globals.h"
#include <iostream>

////////////////////////////////////////////////////////////////////////////////////////////////////
PostProcess::PostProcess()
{
	// Postprocess
	m_colorBuffer = -1;
	m_hColorBuffer = -1;

	// Deferred
	m_PositionBuffer = -1;
	m_NormalBuffer = -1;
	m_AlbedoBuffer = -1;
	m_EmissionBuffer = -1;
	m_MaskBuffer = -1;
	m_SkyboxBuffer = -1;
	m_hPositionBuffer = -1;
	m_hNormalBuffer = -1;
	m_hAlbedoBuffer = -1;
	m_hMaskBuffer = -1;
	m_hSkyboxBuffer = -1;

	m_pPostFXShader = nullptr;
	m_pDeferredShader = nullptr;

	m_pScreenQuadFinal = nullptr;

	m_iChannelID = 0;

	// properties
	m_fExposure = 1.0f;
	m_fGamma = 2.2f;

	m_bDrawBloomEffect = true;
	m_fBloomThreshold = 1.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
PostProcess::~PostProcess()
{
	SAFE_DELETE(m_pDeferredShader);
	SAFE_DELETE(m_pPostFXShader);

	SAFE_DELETE(m_pScreenQuadFinal);
	
	glDeleteBuffers(1, &m_colorBuffer);
	glDeleteBuffers(1, &m_PositionBuffer);
	glDeleteBuffers(1, &m_AlbedoBuffer);
	glDeleteBuffers(1, &m_NormalBuffer);
	glDeleteBuffers(1, &m_MaskBuffer);
	glDeleteBuffers(1, &m_SkyboxBuffer);
	glDeleteBuffers(1, &m_ObjectIDBuffer);
	glDeleteBuffers(1, &m_ShadowDepthBuffer);

	glDeleteFramebuffers(1, &m_fboDeferred);
	glDeleteRenderbuffers(1, &m_rboDeferred);

	glDeleteFramebuffers(1, &m_fboShadow);
	glDeleteRenderbuffers(1, &m_rboShadow);

	glDeleteFramebuffers(1, &m_fboPostFX);
	glDeleteRenderbuffers(1, &m_rboPostFX);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PostProcess::Initialize()
{
	UIManager::getInstance().WriteToConsole(LOGTYPE::LOG_INFO, "PostProcess", "PostProcess initialization started...");

	// Create shader & grab handles to all the uniform variables...
	m_pDeferredShader = new GLSLShader("Shaders/DeferredLighting.vert", "Shaders/DeferredLighting.frag");
	m_pPostFXShader = new GLSLShader("Shaders/PostFX.vert", "Shaders/PostFX.frag");

	// Create screen aligned quad for final render
	m_pScreenQuadFinal = new ScreenAlignedQuad();
	m_pScreenQuadFinal->CreateScreenAlignedQuad(0);

	UIManager::getInstance().WriteToConsole(LOGTYPE::LOG_INFO, "PostProcess", "PostProcess initialization finished...");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PostProcess::CreateDeferredBuffers(int horizRes, int vertRes)
{
	// Create framebuffer object
	glGenFramebuffers(1, &m_fboDeferred);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboDeferred);

	// ! POSITION BUFFER
	// ! RGB - Position & A - Depth
	glGenTextures(1, &m_PositionBuffer);
	glBindTexture(GL_TEXTURE_2D, m_PositionBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, horizRes, vertRes, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_PositionBuffer, 0);

	// ! NORMAL(RGB) + HEIGHT(A) BUFFER
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

	// ! MASK BUFFER
	glGenTextures(1, &m_MaskBuffer);
	glBindTexture(GL_TEXTURE_2D, m_MaskBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, horizRes, vertRes, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, m_MaskBuffer, 0);

	// ! Skybox BUFFER
	glGenTextures(1, &m_SkyboxBuffer);
	glBindTexture(GL_TEXTURE_2D, m_SkyboxBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, horizRes, vertRes, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, m_SkyboxBuffer, 0);

	// ! ObjectID BUFFER
	glGenTextures(1, &m_ObjectIDBuffer);
	glBindTexture(GL_TEXTURE_2D, m_ObjectIDBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, horizRes, vertRes, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D, m_ObjectIDBuffer, 0);

	// create render buffer object 
	glGenRenderbuffers(1, &m_rboDeferred);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rboDeferred);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, horizRes, vertRes);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	
	// attach render buffer to the framebuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rboDeferred);

	// Tell OGL which color attachments we will use
	GLuint attachments[7] = { GL_COLOR_ATTACHMENT0,		// Position
							  GL_COLOR_ATTACHMENT1,		// Normal
							  GL_COLOR_ATTACHMENT2,		// Albedo
		                      GL_COLOR_ATTACHMENT3,		// Emission
	                          GL_COLOR_ATTACHMENT4,		// Mask
							  GL_COLOR_ATTACHMENT5,		// Skybox
							  GL_COLOR_ATTACHMENT6 };	// ObjectID
	glDrawBuffers(7, attachments);

	// Check if correct FBO was created or not ...
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		UIManager::getInstance().WriteToConsole(LOGTYPE::LOG_ERROR, "PostProcess", "Deferred buffer creation failed!");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	UIManager::getInstance().WriteToConsole(LOGTYPE::LOG_INFO, "PostProcess", "Deferred rendering buffers created!");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PostProcess::CreateShadowMappingBuffers(int horizRes, int vertRes)
{
	m_uiShadowDepthBufferWidth = horizRes;
	m_uiShadowDepthBufferHeight = vertRes;

	// create framebuffer object
	glGenFramebuffers(1, &m_fboShadow);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboShadow);

	// Create color texture attachment for this fbo
	glGenTextures(1, &m_ShadowDepthBuffer);
	glBindTexture(GL_TEXTURE_2D, m_ShadowDepthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_uiShadowDepthBufferWidth, m_uiShadowDepthBufferHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindTexture(GL_TEXTURE_2D, 0);

	// attach depth texture buffer to the framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_ShadowDepthBuffer, 0);

	// We only need the depth information when rendering the scene from the light\92s perspective so
	// there is no need for a color buffer.A framebuffer object however is not complete without a color
	// buffer so we need to explicitly tell OpenGL we\92re not going to render any color data.
	glDrawBuffer(GL_NONE);

	// Check if correct FBO was created or not ...
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		UIManager::getInstance().WriteToConsole(LOGTYPE::LOG_ERROR, "PostProcess", "ShadowMap buffer creation failed!");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	UIManager::getInstance().WriteToConsole(LOGTYPE::LOG_INFO, "PostProcess", "Shadow map buffers created!");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PostProcess::CreatePostFXBuffers(int horizRes, int vertRes)
{
	// create framebuffer object
	glGenFramebuffers(1, &m_fboPostFX);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboPostFX);

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
	glGenRenderbuffers(1, &m_rboPostFX);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rboPostFX);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, horizRes, vertRes);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach render buffer to the framebuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rboPostFX);

	// Tell OGL which color attachments we will use
	GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	// Check if correct FBO was created or not ...
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		UIManager::getInstance().WriteToConsole(LOGTYPE::LOG_ERROR, "PostProcess", "PostFX buffer creation failed!");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	UIManager::getInstance().WriteToConsole(LOGTYPE::LOG_INFO, "PostProcess", "PostFX rendering buffers created!");
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
void PostProcess::BeginPostprocessPrepass()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboPostFX);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PostProcess::EndPostprocessPrepass()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PostProcess::BeginShadowPass()
{
	glViewport(0, 0, m_uiShadowDepthBufferWidth, m_uiShadowDepthBufferHeight);
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
	glViewport(0, 0, gWindowWidth, gWindowHeight);
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
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_MaskBuffer);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, m_SkyboxBuffer);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, m_ObjectIDBuffer);

	// Shadow Depth map
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, m_ShadowDepthBuffer);

	// Bind skybox texture
	glActiveTexture(GL_TEXTURE8);
	HDRSkybox::getInstance().BindHDRISKybox();

	// Bind Irrandiance texture
	glActiveTexture(GL_TEXTURE9);
	HDRSkybox::getInstance().BindIrrandianceSkybox();

	// Bind Prefiltered Specular Map
	glActiveTexture(GL_TEXTURE10);
	HDRSkybox::getInstance().BindPrefilteredSpecularMap();

	// Bind BRDF LUT Map
	glActiveTexture(GL_TEXTURE11);
	HDRSkybox::getInstance().BindBrdfLUTMap();

	// Render Quad!
	m_pScreenQuadFinal->RenderToScreenAlignedQuad();

	// Unbind all textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE8);
	HDRSkybox::getInstance().UnbindHDRISkybox();
	glActiveTexture(GL_TEXTURE9);
	HDRSkybox::getInstance().UnbindIrrandianceSkybox();
	glActiveTexture(GL_TEXTURE10);
	HDRSkybox::getInstance().UnbindPrefilteredSpecularMap();
	glActiveTexture(GL_TEXTURE11);
	HDRSkybox::getInstance().UnbindBrdfLUTMap();
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

	m_hBrightBuffer = glGetUniformLocation(id, "brightBuffer");
	glUniform1i(m_hBrightBuffer, 1);

	GLint m_hGamma = glGetUniformLocation(id, "gamma");
	glUniform1f(m_hGamma, m_fGamma);
	
	GLint m_hExposure = glGetUniformLocation(id, "exposure");
	glUniform1f(m_hExposure, m_fExposure);

	glUniform1i(glGetUniformLocation(id, "DoBloom"), m_bDrawBloomEffect);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_colorBuffer);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_brightBuffer);

	m_pScreenQuadFinal->RenderToScreenAlignedQuad();

	glBindTexture(GL_TEXTURE_2D, 0);
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
		glm::vec3 color = light->GetLightColor();
		float intensity = light->m_fLightIntensity;
		float radius = light->m_fLightRadius;

		// form a string out of point light Ids
		std::string pointLightPosStr = "pointLights[" + std::to_string(i) + "].position";
		std::string pointLightColStr = "pointLights[" + std::to_string(i) + "].color";
		std::string pointLightIntStr = "pointLights[" + std::to_string(i) + "].intensity";
		std::string pointLightRadStr = "pointLights[" + std::to_string(i) + "].radius";

		glUniform3fv(glGetUniformLocation(shaderID, pointLightPosStr.c_str()), 1, glm::value_ptr(position));
		glUniform3fv(glGetUniformLocation(shaderID, pointLightColStr.c_str()), 1, glm::value_ptr(color));
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

		glm::vec3 direction = light->m_vecLightDirection;
		glm::vec3 color = light->m_vecLightColor;
		float intensity = light->m_fLightIntensity;

		// form a string out of directional light IDs
		std::string dirLightDirectionStr = "dirLights[" + std::to_string(i) + "].direction";
		std::string dirLightColStr		 = "dirLights[" + std::to_string(i) + "].color";
		std::string dirLightIntensity	 = "dirLights[" + std::to_string(i) + "].intensity";

		glUniform3fv(glGetUniformLocation(shaderID, dirLightDirectionStr.c_str()), 1, glm::value_ptr(direction));
		glUniform3fv(glGetUniformLocation(shaderID, dirLightColStr.c_str()), 1, glm::value_ptr(color));
		glUniform1f(glGetUniformLocation(shaderID, dirLightIntensity.c_str()), intensity);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PostProcess::SetDeferredPassShaderVariables(int shaderID)
{
	glm::vec3 camPosition = Camera::getInstance().getCameraPosition();
	glm::mat4 camViewMatrix = Camera::getInstance().getViewMatrix();
	glm::mat4 camProjMatrix = Camera::getInstance().getProjectionMatrix();

	glm::mat4 worldToLightViewMatrix = LightsManager::getInstance()->GetDirectionalLight(0)->m_matWorldToLightViewMatrix;
	glm::mat4 lightViewToProjectionMatrix = LightsManager::getInstance()->GetDirectionalLight(0)->m_matLightViewToProjectionMatrix;

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
	m_hMaskBuffer = glGetUniformLocation(shaderID, "maskBuffer");
	m_hSkyboxBuffer = glGetUniformLocation(shaderID, "skyboxBuffer");
	m_hObjectIDBuffer = glGetUniformLocation(shaderID, "objectIDBuffer");
	
	m_hShadowDepthBuffer = glGetUniformLocation(shaderID, "shadowDepthBuffer");

	// Bloom
	GLuint hBloomThreshold = glGetUniformLocation(shaderID, "fBloomThreshold");
	glUniform1f(hBloomThreshold, m_fBloomThreshold);

	// Skybox map
	GLuint hCubeMap = glGetUniformLocation(shaderID, "texture_skybox");

	// Skybox Irradiance map
	GLuint hIrradMap = glGetUniformLocation(shaderID, "texture_irradiance");

	// Skybox Prefiltered specular map
	GLuint hPrefiltSpecMap = glGetUniformLocation(shaderID, "texture_prefiltSpecular");

	// BRDF LUT map
	GLuint hBrdfLUT = glGetUniformLocation(shaderID, "texture_brdfLUT");

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
	glUniform1i(m_hMaskBuffer, 4);
	glUniform1i(m_hSkyboxBuffer, 5);
	glUniform1i(m_hObjectIDBuffer, 6);
	glUniform1i(m_hShadowDepthBuffer, 7);
	glUniform1i(hCubeMap, 8);
	glUniform1i(hIrradMap, 9);
	glUniform1i(hPrefiltSpecMap, 10);
	glUniform1i(hBrdfLUT, 11);

	// set channel ID to draw!
	glUniform1i(glGetUniformLocation(shaderID, "channelID"), m_iChannelID);

	// Point Light related uniform variables!
	PointLightIlluminance(shaderID);

	// Directional Light related uniform variables!
	DirectionalLightIlluminance(shaderID);
}


