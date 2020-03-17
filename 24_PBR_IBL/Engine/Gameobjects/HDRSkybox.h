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

	void		BindIrrandianceSkybox();
	void		UnbindIrrandianceSkybox();

	void		BindPrefilteredSpecularMap();
	void		UnbindPrefilteredSpecularMap();

	void		BindBrdfLUTMap();
	void		UnbindBrdfLUTMap();

private:
	HDRSkybox();
	HDRSkybox(const HDRSkybox&);
	void operator=(const HDRSkybox&);

	void		InitCaptureCube();
	void		InitLUTQuad();
	void		InitCaptureCubemap();
	void		InitIrradianceCubemap();
	void		InitPrefilteredSpecularCubemap();
	void		InitSpecularBrdfLUT();
	void		RenderCubemap();
	void		RenderQuad();

	GLuint		m_vao;
	GLuint		m_vbo;
	GLuint		m_ibo;
	GLuint		m_tbo;

	VertexP		m_Vertices[8];
	GLuint		m_Indices[36];
	GLuint		m_posAttrib;

	GLuint		m_captureFBO;			// FBO for capturing cubemap
	GLuint		m_captureRBO;			// RBO for capturing cubemap
	GLuint		m_captureTBO;			// TBO for cubemap
	GLuint		m_IrradianceTBO;		// TBO for irradinace map
	GLuint		m_PrefilterSpecmapTBO;	// TBO for pre-filtered specmap 
	GLuint		m_BrdfLUTmapTBO;		// TBO for brdf LUT map

	int			m_iCubemapSize;			// capture cubemap size

	GLuint		m_Quad_vao;				// VAO for quad
	GLuint		m_Quad_vbo;				// VBO for quad
	GLuint		m_Quad_ibo;				// IBO for quad
	GLint		m_Quad_PosAttrib;		// vertex attribute id
	GLint		m_Quad_UVAttrib;		// texcoord attribute id
	VertexPT	m_Quad_Vertices[4];		// vertices of a cube
	GLuint		m_Quad_Indices[6];		// indices of a cube

	GLint		m_hWorld;
	GLint		m_hView;
	GLint		m_hProj;

	glm::mat4	m_matCaptureProj;
	glm::mat4	m_matCaptureView[6];
	glm::vec3	m_vecPosition;

	glm::mat4	m_matWorld;
	glm::mat4	m_matView;
	glm::mat4	m_matProj;

	GLSLShader* m_pRenderShader;		// Ptr to render shader (Cubemap as skybox)
	GLSLShader* m_pCaptureShader;		// Ptr to capture shader (HDRI --> Cubemap)
	GLSLShader* m_pIrradianceShader;	// Ptr to Irradiance shader (Cubemap --> Irradiance map)
	GLSLShader* m_pPrefiltSpecShader;	// Ptr to Prefilter Specmap shader (cubemap --> Prefilter spec map)
	GLSLShader* m_pBrdfLUTShader;		// Ptr to Specular BRDF LUT generator
};
