#include "HDRSkybox.h"
#include "Camera.h"
#include "../Main/Globals.h"
#include "../Main/Helper.h"
#include "../Rendering/GLSLShader.h"
#include "../Rendering/TextureManager.h"
#include "../UI/UIManager.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
HDRSkybox::~HDRSkybox()
{
	SAFE_DELETE(m_pCaptureShader);
	SAFE_DELETE(m_pRenderShader);
	SAFE_DELETE(m_pBrdfLUTShader);
	SAFE_DELETE(m_pIrradianceShader);
	SAFE_DELETE(m_pPrefiltSpecShader);

	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ibo);
	glDeleteTextures(1, &m_tbo);

	glDeleteBuffers(1, &m_Quad_vbo);
	glDeleteBuffers(1, &m_Quad_ibo);

	glDeleteVertexArrays(1, &m_vao);
	glDeleteVertexArrays(1, &m_Quad_vao);

	glDeleteFramebuffers(1, &m_captureFBO);
	glDeleteRenderbuffers(1, &m_captureRBO);
	glDeleteTextures(1, &m_captureTBO);
	glDeleteTextures(1, &m_BrdfLUTmapTBO);
	glDeleteTextures(1, &m_IrradianceTBO);
	glDeleteTextures(1, &m_PrefilterSpecmapTBO);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HDRSkybox::HDRSkybox()
{
	m_iCubemapSize = 512;

	m_iBrdfLUTmapSize = 512;
	m_iIrradiancemapSize = 32;
	m_iPrefiltCubemapSize = 128;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void HDRSkybox::Initialize()
{
	m_pCaptureShader = new GLSLShader("Shaders/HDRI2Cubemap.vert", "Shaders/HDRI2Cubemap.frag");
	m_pRenderShader = new GLSLShader("Shaders/HDRISkybox.vert", "Shaders/HDRISkybox.frag");
	m_pIrradianceShader = new GLSLShader("Shaders/Cubemap2Irradiance.vert", "Shaders/Cubemap2Irradiance.frag");
	m_pPrefiltSpecShader = new GLSLShader("Shaders/PrefilterSpecmap.vert", "Shaders/PrefilterSpecmap.frag");
	m_pBrdfLUTShader = new GLSLShader("Shaders/BRDFLut.vert", "Shaders/BRDFLut.frag");

	m_tbo = TextureManager::getInstannce().Load2DTextureFromFile("uffizi.hdr", "../Assets/HDRI");

	// create cube for capturing cubemap
	InitCaptureCube();

	// create quad for Specular BRDF LUT texture
	InitLUTQuad();

	// capture cubemap for HDRI map
	InitCaptureCubemap();

	// create Irradiance map from cubemap
	InitIrradianceCubemap();

	// create Prefiltered specular cubemap
	InitPrefilteredSpecularCubemap();

	// create Specular BRDF LUT Map
	InitSpecularBrdfLUT();

	UIManager::getInstance().WriteToConsole(LOGTYPE::LOG_INFO, "HDRSkybox", "HDRI Skybox initialized...");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void HDRSkybox::InitCaptureCube()
{
	// Vertex Data
	m_Vertices[0] = VertexP(glm::vec3(-1, -1, 1));
	m_Vertices[1] = VertexP(glm::vec3(1, -1, 1));
	m_Vertices[2] = VertexP(glm::vec3(1, 1, 1));
	m_Vertices[3] = VertexP(glm::vec3(-1, 1, 1));
	m_Vertices[4] = VertexP(glm::vec3(-1, -1, -1));
	m_Vertices[5] = VertexP(glm::vec3(1, -1, -1));
	m_Vertices[6] = VertexP(glm::vec3(1, 1, -1));
	m_Vertices[7] = VertexP(glm::vec3(-1, 1, -1));

	// Index data ( 6 faces = 12 triangles = 36 indices )
	m_Indices[0] = 0;			m_Indices[1] = 1;		m_Indices[2] = 2;
	m_Indices[3] = 2;			m_Indices[4] = 3;		m_Indices[5] = 0;

	m_Indices[6] = 3;			m_Indices[7] = 2;		m_Indices[8] = 6;
	m_Indices[9] = 6;			m_Indices[10] = 7;		m_Indices[11] = 3;

	m_Indices[12] = 7;			m_Indices[13] = 6;		m_Indices[14] = 5;
	m_Indices[15] = 5;			m_Indices[16] = 4;		m_Indices[17] = 7;

	m_Indices[18] = 4;			m_Indices[19] = 5;		m_Indices[20] = 1;
	m_Indices[21] = 1;			m_Indices[22] = 0;		m_Indices[23] = 4;

	m_Indices[24] = 4;			m_Indices[25] = 0;		m_Indices[26] = 3;
	m_Indices[27] = 3;			m_Indices[28] = 7;		m_Indices[29] = 4;

	m_Indices[30] = 1;			m_Indices[31] = 5;		m_Indices[32] = 6;
	m_Indices[33] = 6;			m_Indices[34] = 2;		m_Indices[35] = 1;

	// create vao
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// create vbo
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(VertexP), m_Vertices, GL_STATIC_DRAW);

	// create ibo
	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Indices), m_Indices, GL_STATIC_DRAW);

	GLuint shader = m_pCaptureShader->GetShaderID();
	m_posAttrib = glGetAttribLocation(shader, "in_Position");
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(VertexP), (void*)0);

	glBindVertexArray(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void HDRSkybox::InitLUTQuad()
{
	// Quad vertex data
	m_Quad_Vertices[0] = VertexPT(glm::vec3(-1, -1, 0), glm::vec2(0.0f, 0.0f));
	m_Quad_Vertices[1] = VertexPT(glm::vec3(-1, 1, 0), glm::vec2(0.0f, 1.0f));
	m_Quad_Vertices[2] = VertexPT(glm::vec3(1, 1, 0), glm::vec2(1.0f, 1.0f));
	m_Quad_Vertices[3] = VertexPT(glm::vec3(1, -1, 0), glm::vec2(1.0f, 0.0f));

	// Quad Index data ( 1 face = 2 triangles = 6 indices )
	m_Quad_Indices[0] = 0;	m_Quad_Indices[1] = 1;	m_Quad_Indices[2] = 2;
	m_Quad_Indices[3] = 2;	m_Quad_Indices[4] = 3;	m_Quad_Indices[5] = 0;

	// create vao
	glGenVertexArrays(1, &m_Quad_vao);
	glBindVertexArray(m_Quad_vao);

	// create vbo
	glGenBuffers(1, &m_Quad_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_Quad_vbo);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(VertexPT), m_Quad_Vertices, GL_STATIC_DRAW);

	// create ibo
	glGenBuffers(1, &m_Quad_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Quad_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Quad_Indices), m_Quad_Indices, GL_STATIC_DRAW);

	GLuint shader = m_pBrdfLUTShader->GetShaderID();
	m_Quad_PosAttrib = glGetAttribLocation(shader, "in_Position");
	glEnableVertexAttribArray(m_Quad_PosAttrib);
	glVertexAttribPointer(m_Quad_PosAttrib, 3, GL_FLOAT, false, sizeof(VertexPT), (void*)0);

	m_Quad_UVAttrib = glGetAttribLocation(shader, "in_Texcoord");
	glEnableVertexAttribArray(m_Quad_UVAttrib);
	glVertexAttribPointer(m_Quad_UVAttrib, 2, GL_FLOAT, false, sizeof(VertexPT), (void*)offsetof(VertexPT, uv));

	//glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void HDRSkybox::InitCaptureCubemap()
{
	// Create FBO & assign correct RBO to it
	glGenFramebuffers(1, &m_captureFBO);
	glGenRenderbuffers(1, &m_captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, m_captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_iCubemapSize, m_iCubemapSize);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_captureRBO);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//------------------------------------- HDRI to Cubemap -------------------------------------//
	// Create TBO (In this case, it will hold 6 textures for cubemap)
	glGenTextures(1, &m_captureTBO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_captureTBO);

	for (GLuint i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, m_iCubemapSize, m_iCubemapSize, 0, GL_RGB, GL_FLOAT, nullptr);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// set up projection & view matrices for capturing data onto 6 faces of cubemap from 6 directions!!!
	m_matCaptureProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

	m_matCaptureView[0] = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(-1, 0, 0), glm::vec3(0, 1, 0));
	m_matCaptureView[1] = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));
	m_matCaptureView[2] = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, -1, 0), glm::vec3(0, 0, 1));
	m_matCaptureView[3] = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, -1));
	m_matCaptureView[4] = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
	m_matCaptureView[5] = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));

	// Convert HDR equirectangular map to cubemap equivalent...
	m_pCaptureShader->Use();

	GLuint shader = m_pCaptureShader->GetShaderID();

	// set HDRI as active texture input to shader
	glUniform1i(glGetUniformLocation(shader, "HDRITexture"), 0);

	// set capture projection & view matrices to capture all 6 faces...
	glUniformMatrix4fv(glGetUniformLocation(shader, "matProj"), 1, GL_FALSE, glm::value_ptr(m_matCaptureProj));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_tbo);

	// Configure viewport to capture dimension
	glViewport(0, 0, m_iCubemapSize, m_iCubemapSize);

	// Bind capture framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO);
	
	for (GLuint i = 0; i < 6; ++i)
	{
		glUniformMatrix4fv(glGetUniformLocation(shader, "matView"), 1, GL_FALSE, glm::value_ptr(m_matCaptureView[i]));

		// capture texture at 6 directions & store into cubemap texture object
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_captureTBO, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(m_vao);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// let OpenGL generate mipmaps from first mip face(combatting visible dots artifact)
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_captureTBO);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	// Restore original viewport dimensions!
	glViewport(0, 0, gWindowWidth, gWindowHeight);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void HDRSkybox::InitIrradianceCubemap()
{
	// Generate Irradiance map from already created cubemap
	// 1. Create empty faces for irradiance map, in this case, we need 6!
	glGenTextures(1, &m_IrradianceTBO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_IrradianceTBO);

	for (GLuint i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, m_iIrradiancemapSize, m_iIrradiancemapSize, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// 2. start by binding this cubemap to capture FBO & running convolution shader
	glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, m_captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_iIrradiancemapSize, m_iIrradiancemapSize);

	m_pIrradianceShader->Use();
	GLuint shaderID = m_pIrradianceShader->GetShaderID();

	// set captured cubemap as input texture
	glUniform1i(glGetUniformLocation(shaderID, "CubemapTexture"), 0);

	// set capture projection & view matrices for all faces...
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "matProj"), 1, GL_FALSE, glm::value_ptr(m_matCaptureProj));

	// set capture cubemap 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_captureTBO);

	// Configure viewport to capture dimension
	glViewport(0, 0, m_iIrradiancemapSize, m_iIrradiancemapSize);

	// perform convolution on each face
	for (GLuint j = 0; j < 6; ++j)
	{
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "matView"), 1, GL_FALSE, glm::value_ptr(m_matCaptureView[j]));

		// capture texture at 6 directions & store into cubemap texture object
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, m_IrradianceTBO, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(m_vao);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Restore original viewport dimensions!
	glViewport(0, 0, gWindowWidth, gWindowHeight);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void HDRSkybox::InitPrefilteredSpecularCubemap()
{
	// Generate Prefiltered specular map from already created cubemap
	// 1. Create empty faces for prefilterd specular map, in this case, we need 6!
	glGenTextures(1, &m_PrefilterSpecmapTBO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_PrefilterSpecmapTBO);
	for (GLuint i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, m_iPrefiltCubemapSize, m_iPrefiltCubemapSize, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	// set shader for usage
	m_pPrefiltSpecShader->Use();
	GLuint shaderID = m_pPrefiltSpecShader->GetShaderID();

	// set captured cubemap as an active texture
	glUniform1i(glGetUniformLocation(shaderID, "CubemapTexture"), 0);

	// set capture projection & view matrices for all faces...
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "matProj"), 1, GL_FALSE, glm::value_ptr(m_matCaptureProj));

	// now start the process of creating 6 faces of cubemap. Each face will have 5 levels of mipmaps based on roughness level.
	glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO);

	// set capture cubemap 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_captureTBO);

	unsigned int maxMipLevels = 5;
	for (GLuint mip = 0; mip < maxMipLevels; ++mip)
	{
		// resize framebuffer based on mip level
		unsigned int mipWidth =  m_iPrefiltCubemapSize * std::pow(0.5f, mip);
		unsigned int mipHeight = m_iPrefiltCubemapSize * std::pow(0.5f, mip);

		glBindRenderbuffer(GL_RENDERBUFFER, m_captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		glUniform1f(glGetUniformLocation(shaderID, "roughness"), roughness);

		for (GLuint j = 0; j < 6; ++j)
		{
			glUniformMatrix4fv(glGetUniformLocation(shaderID, "matView"), 1, GL_FALSE, glm::value_ptr(m_matCaptureView[j]));
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, m_PrefilterSpecmapTBO, mip);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glBindVertexArray(m_vao);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, gWindowWidth, gWindowHeight);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void HDRSkybox::InitSpecularBrdfLUT()
{
	glGenTextures(1, &m_BrdfLUTmapTBO);
	glBindTexture(GL_TEXTURE_2D, m_BrdfLUTmapTBO);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, m_iBrdfLUTmapSize, m_iBrdfLUTmapSize, 0, GL_RG, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, m_captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_iBrdfLUTmapSize, m_iBrdfLUTmapSize);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_BrdfLUTmapTBO, 0);

	glViewport(0, 0, m_iBrdfLUTmapSize, m_iBrdfLUTmapSize);

	m_pBrdfLUTShader->Use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	RenderQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, gWindowWidth, gWindowHeight);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void HDRSkybox::RenderCubemap()
{
	// So based on shader for skybox, we have made sure that third component equal to w, 
	// perspective divide always returns 1. This way depth value is always 1 which is at
	// back of all the geometries rendered. 
	// The depth buffer will be filled with values of 1.0 for the skybox, so we need to 
	// make sure the skybox passes the depth tests with values less than or equal to the 
	// depth buffer instead of less than. 
	glDepthFunc(GL_LEQUAL);
	m_pRenderShader->Use();

	GLuint shader = m_pRenderShader->GetShaderID();

	glUniform1i(glGetUniformLocation(shader, "cubeMap"), 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_captureTBO);

	m_matView = glm::mat4(glm::mat3(Camera::getInstance().getViewMatrix()));
	m_matProj = Camera::getInstance().getProjectionMatrix();

	glUniformMatrix4fv(glGetUniformLocation(shader, "matView"), 1, GL_FALSE, glm::value_ptr(m_matView));
	glUniformMatrix4fv(glGetUniformLocation(shader, "matProj"), 1, GL_FALSE, glm::value_ptr(m_matProj));

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// Set depth function back to default
	glDepthFunc(GL_LESS);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void HDRSkybox::RenderQuad()
{
	glBindVertexArray(m_Quad_vao);
	// know more about the last element!
	// http://stackoverflow.com/questions/17191258/no-display-from-gldrawelements
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void HDRSkybox::Update(float dt)
{
	m_matWorld = glm::translate(glm::mat4(1), glm::vec3(0, 0, 0));

	// Note that, we first convert matrix to 3x3 so that we remove the translation part, later
	// we convert back to 4x4 matrix keeping the rotation part intact! 
	// All this is done so that, when player moves, skybox moves with him giving impression that
	// skybox is far far away!
	m_matView = glm::mat4(glm::mat3(Camera::getInstance().getViewMatrix()));

	m_matProj = Camera::getInstance().getProjectionMatrix();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void HDRSkybox::Render()
{
	RenderCubemap();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void HDRSkybox::BindHDRISKybox()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_captureTBO);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void HDRSkybox::UnbindHDRISkybox()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void HDRSkybox::BindIrrandianceSkybox()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_IrradianceTBO);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void HDRSkybox::UnbindIrrandianceSkybox()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void HDRSkybox::BindPrefilteredSpecularMap()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_PrefilterSpecmapTBO);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void HDRSkybox::UnbindPrefilteredSpecularMap()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void HDRSkybox::BindBrdfLUTMap()
{
	glBindTexture(GL_TEXTURE_2D, m_BrdfLUTmapTBO);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void HDRSkybox::UnbindBrdfLUTMap()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}
