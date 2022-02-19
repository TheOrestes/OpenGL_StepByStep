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
	void			ExecutePostprocessPass();
	void			ExecuteBloomPass();

	void			CreateColorDepthStencilBuffer(int horizRes, int vertRes);
	void			CreateColorBrightnessBuffer(int horizRes, int vertRes);

	void			SetWireframe(bool flag);

private:
	PostProcess(const PostProcess&);
	void operator=(const PostProcess&);

	

	GLuint			m_vbo;				// vertex buffer object
	GLuint			m_vao;				// vertex array object
	GLuint			m_colorBuffer;		// texture object for holding color data
	GLuint			m_brightBuffer;		// texture object for holding brightness data
	GLint			m_hColorBuffer;		// handle to screen texture object
	GLint			m_hBrightBuffer;	// handle to brightness texture object

	// Framebuffer stuff...
	GLuint			m_fbo;				// Framebuffer object
	GLuint			m_rbo;				// Renderbuffer object
	

	GLint			m_posAttrib;		// vertex attribute id
	GLint			m_texAttrib;		// color attribute id
	VertexPT		m_vertices[6];		// vertices of a quad

	bool			m_bWireframe;		// wire-frame rendering flag

	GLSLShader*		m_pShader;			// Ptr to shader object

	// Properties
	float			m_fGamma;
	float			m_fExposure;
};

