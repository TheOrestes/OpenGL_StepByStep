#pragma once


#pragma once

#include "GL\glew.h"
#include "VertexStructures.h"

class GLSLShader;

class PostProcess
{
public:
	PostProcess();
	~PostProcess();

	void			SetupScreenQuad();

	void			BeginRTPass();
	void			EndRTPass();
	void			BeginGeometryRenderPass();
	void			EndGeometryRenderPass();
	void			BeginBloomPrepass();
	void			EndBloomPrepass();
	void			BeginShadowPass();
	void			EndShadowPass();

	void			ExecuteDeferredRenderPass();
	void			ExecutePostprocessPass();
	void			ExecuteBloomPass();

	void			CreateDeferredBuffers(int horizRes, int vertRes);
	void			CreateShadowMappingBuffers(int horizRes, int vertRes);
	void			CreateBloomBuffers(int horizRes, int vertRes);

	void			SetWireframe(bool flag);

private:
	PostProcess(const PostProcess&);
	void operator=(const PostProcess&);

	void			PointLightIlluminance(int shaderID);
	void			DirectionalLightIlluminance(int shaderID);
	void			SetDeferredPassShaderVariables(int shaderID);

	// Screen aligned Quad data
	GLuint			m_vbo;						// vertex buffer object
	GLuint			m_vao;						// vertex array object
	GLint			m_posAttrib;				// vertex attribute id
	GLint			m_texAttrib;				// color attribute id
	VertexPT		m_vertices[6];				// vertices of a quad

	// PostProcess
	GLuint			m_colorBuffer;				// texture object for holding color data
	GLuint			m_brightBuffer;				// texture object for holding brightness data

	GLint			m_hColorBuffer;				// handle to screen texture object
	GLint			m_hBrightBuffer;			// handle to brightness texture object

	// Deferred Rendering 
	GLuint			m_fboDeferred;				// framebuffer object for Deferred buffers
	GLuint			m_rboDeferred;				// renderbuffer object for deferred buffers

	GLuint			m_PositionBuffer;			// Position buffer
	GLuint			m_AlbedoBuffer;				// Albedo color buffer
	GLuint			m_NormalBuffer;				// Normal information buffer
	GLuint			m_EmissionBuffer;			// Emissive information buffer
	GLuint			m_DepthBuffer;				// Depth buffer

	GLint			m_hPositionBuffer;			// Handle to Position buffer
	GLint			m_hAlbedoBuffer;			// Handle to Albedo color buffer
	GLint			m_hNormalBuffer;			// Handle to Normal information buffer
	GLint			m_hEmissiveBuffer;			// Handle to Emissive buffer
	GLint			m_hDepthBuffer;				// Handle to Depth buffer

	// Shadow Mapping
	GLuint			m_fboShadow;				// Framebuffer object for shadow mapping
	GLuint			m_rboShadow;				// Renderbuffer object for shadow mapping

	GLuint			m_ShadowDepthBuffer;		// Depth buffer
	GLuint			m_hShadowDepthBuffer;		// Handle to Depth buffer
	uint16_t		m_uiShadowDepthBufferWidth;
	uint16_t		m_uiShadowDepthBufferHeight;

	// Postprocessing
	GLuint			m_fboPostProcess;			// Framebuffer object for postprocess
	GLuint			m_rboPostProcess;			// Renderbuffer object for postprocess

	// Bloom
	GLuint			m_fboBloom;					// Framebuffer object for Bloom
	GLuint			m_rboBloom;					// Renderbuffer object for Bloom

	bool			m_bWireframe;				// wire-frame rendering flag

	GLSLShader*		m_pPostFXShader;			// Ptr to Postprocess shader
	GLSLShader*		m_pDeferredShader;			// Ptr to Deferred shader
	GLSLShader*		m_pBloomShader;				// Ptr to Bloom shader

	// Postprocess properties
	float			m_fGamma;
	float			m_fExposure;

};

