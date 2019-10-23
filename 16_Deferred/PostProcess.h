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
	void			ExecuteDeferredRenderPass();
	void			ExecutePostprocessPass();
	void			ExecuteBloomPass();

	void			CreateDeferredBuffers(int horizRes, int vertRes);
	void			CreateColorDepthStencilBuffer(int horizRes, int vertRes);
	void			CreateColorBrightnessBuffer(int horizRes, int vertRes);

	void			SetWireframe(bool flag);

private:
	PostProcess(const PostProcess&);
	void operator=(const PostProcess&);

	void			PointLightIlluminance(int shaderID);
	void			SetDeferredPassShaderVariables(int shaderID);

	// Screen aligned Quad data
	GLuint			m_vbo;				// vertex buffer object
	GLuint			m_vao;				// vertex array object
	GLint			m_posAttrib;		// vertex attribute id
	GLint			m_texAttrib;		// color attribute id
	VertexPT		m_vertices[6];		// vertices of a quad

	// PostProcess
	GLuint			m_colorBuffer;		// texture object for holding color data
	GLuint			m_brightBuffer;		// texture object for holding brightness data

	GLint			m_hColorBuffer;		// handle to screen texture object
	GLint			m_hBrightBuffer;	// handle to brightness texture object

	// Deferred Rendering 
	GLuint			m_fboDeferred;		// framebuffer object for Deferred buffers
	GLuint			m_rboDeferred;		// renderbuffer object for deferred buffers

	GLuint			m_PositionBuffer;	// Position buffer
	GLuint			m_AlbedoBuffer;		// Albedo color buffer
	GLuint			m_NormalBuffer;		// Normal information buffer

	GLint			m_hPositionBuffer;	// Handle to Position buffer
	GLint			m_hAlbedoBuffer;	// Handle to Albedo color buffer
	GLint			m_hNormalBuffer;	// Handle to Normal information buffer

	// Postprocessing
	GLuint			m_fboPostProcess;	// Framebuffer object for postprocess
	GLuint			m_rboPostProcess;	// Renderbuffer object for postprocess
	

	bool			m_bWireframe;		// wire-frame rendering flag

	GLSLShader*		m_pShader;			// Ptr to shader object

	// Postprocess properties
	float			m_fGamma;
	float			m_fExposure;
};

