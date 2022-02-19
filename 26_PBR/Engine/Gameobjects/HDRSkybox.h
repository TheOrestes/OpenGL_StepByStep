#pragma once

#include "../Rendering/VertexStructures.h"

class GLSLShader;

class HDRSkybox
{
public:
	static HDRSkybox& getInstance()
	{
		static HDRSkybox instance;
		return instance;
	}

	~HDRSkybox();

	void		Initialize();
	void		Update(float dt);
	void		Render();
	void		BindHDRISKybox();
	void		UnbindHDRISkybox();

private:
	HDRSkybox();
	HDRSkybox(const HDRSkybox&);
	void operator=(const HDRSkybox&);

	void		InitCaptureCube();
	void		InitCaptureCubemap();
	void		RenderCubemap();

	GLuint		m_vao;
	GLuint		m_vbo;
	GLuint		m_ibo;
	GLuint		m_tbo;

	VertexP		m_Vertices[8];
	GLuint		m_Indices[36];
	GLuint		m_posAttrib;

	GLuint		m_captureFBO;	// FBO for capturing cubemap
	GLuint		m_captureRBO;	// RBO for capturing cubemap
	GLuint		m_captureTBO;	// TBO for cubemap

	int			m_iCubemapSize;	// capture cubemap size

	GLint		m_hWorld;
	GLint		m_hView;
	GLint		m_hProj;

	glm::mat4	m_matCaptureProj;
	glm::mat4	m_matCaptureView[6];
	glm::vec3	m_vecPosition;

	glm::mat4	m_matWorld;
	glm::mat4	m_matView;
	glm::mat4	m_matProj;

	GLSLShader* m_pRenderShader;	// Ptr to render shader (Cubemap as skybox)
	GLSLShader* m_pCaptureShader;	// Ptr to capture shader (HDRI --> Cubemap)
};
