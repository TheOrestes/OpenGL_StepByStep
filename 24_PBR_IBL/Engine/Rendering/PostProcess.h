#pragma once

#include "GL\glew.h"
#include "VertexStructures.h"

class ScreenAlignedQuad;
class GLSLShader;

class PostProcess
{
public:
	PostProcess();
	~PostProcess();

	void				Initialize();

	void				BeginGeometryRenderPass();
	void				EndGeometryRenderPass();
	void				BeginPostprocessPrepass();
	void				EndPostprocessPrepass();
	void				BeginShadowPass();
	void				EndShadowPass();

	void				ExecuteDeferredRenderPass();
	void				ExecutePostprocessPass();

	void				CreateDeferredBuffers(int horizRes, int vertRes);
	void				CreateShadowMappingBuffers(int horizRes, int vertRes);
	void				CreatePostFXBuffers(int horizRes, int vertRes);

	void				DrawDebugBuffers();

	//--- GETTER-SETTER
	inline	void		SetDebugDrawGBuffers(bool _flag)	{ m_bDrawDebugBuffers = _flag; }
	inline	void		SetBloomEffect(bool _flag)			{ m_bDrawBloomEffect = _flag; }
	inline	void		SetBloomThreshold(float _value)		{ m_fBloomThreshold = _value; }
	inline	void		SetGamma(float _value)				{ m_fGamma = _value; }
	inline	void		SetExposure(float _value)			{ m_fExposure = _value; }

	inline	bool		GetBloomStatus()					{ return m_bDrawBloomEffect; }
	inline	float		GetBloomThreshold()					{ return m_fBloomThreshold; }
	inline	float		GetGamma()							{ return m_fGamma; }
	inline	float		GetExposure()						{ return m_fExposure; }

private:
	PostProcess(const PostProcess&);
	void operator=(const PostProcess&);

	void				PointLightIlluminance(int shaderID);
	void				DirectionalLightIlluminance(int shaderID);
	void				SetDeferredPassShaderVariables(int shaderID);

	ScreenAlignedQuad*	m_pScreenQuadFinal;			// Main screen aligned quad used for rendering!

	ScreenAlignedQuad*	m_pDebugQuadPosition;		// Debug Position
	ScreenAlignedQuad*	m_pDebugQuadNormal;			// Debug Normal
	ScreenAlignedQuad*	m_pDebugQuadAlbedo;			// Debug Albedo
	ScreenAlignedQuad*  m_pDebugQuadEmission;		// Debug Emission
	ScreenAlignedQuad*	m_pDebugQuadObjectID;		// Debug ObjectID
	ScreenAlignedQuad*  m_pDebugQuadBrightness;		// Debug Brightness
	ScreenAlignedQuad*  m_pDebugQuadShadowDepth;	// Debug Shadow Depth

	// PostProcess
	GLuint				m_colorBuffer;				// texture object for holding color data
	GLuint				m_brightBuffer;				// texture object for holding brightness data

	GLint				m_hColorBuffer;				// handle to screen texture object
	GLint				m_hBrightBuffer;			// handle to brightness texture object

	// Deferred Rendering 
	GLuint				m_fboDeferred;				// framebuffer object for Deferred buffers
	GLuint				m_rboDeferred;				// renderbuffer object for deferred buffers

	GLuint				m_PositionBuffer;			// Position buffer
	GLuint				m_AlbedoBuffer;				// Albedo color buffer
	GLuint				m_NormalBuffer;				// Normal information buffer
	GLuint				m_EmissionBuffer;			// Emissive information buffer
	GLuint				m_MaskBuffer;				// RGB mask buffer, R:Roughness,G:Metallic,B:Occlusion
	GLuint				m_SkyboxBuffer;				// Skybox buffer
	GLuint				m_ObjectIDBuffer;			// Object ID buffer, Red - StaticObjects, Green - Skybox, Blue - Unused


	GLint				m_hPositionBuffer;			// Handle to Position buffer
	GLint				m_hAlbedoBuffer;			// Handle to Albedo color buffer
	GLint				m_hNormalBuffer;			// Handle to Normal information buffer
	GLint				m_hEmissiveBuffer;			// Handle to Emissive buffer
	GLint				m_hMaskBuffer;				// Handle to Mask buffer
	GLint				m_hSkyboxBuffer;			// Handle to skybox buffer
	GLint				m_hObjectIDBuffer;			// Handle to ObjectID buffer

	// Shadow Mapping
	GLuint				m_fboShadow;				// Framebuffer object for shadow mapping
	GLuint				m_rboShadow;				// Renderbuffer object for shadow mapping

	GLuint				m_ShadowDepthBuffer;		// Depth buffer
	GLuint				m_hShadowDepthBuffer;		// Handle to Depth buffer

	// Postprocessing
	GLuint				m_fboPostFX;			// Framebuffer object for postprocess
	GLuint				m_rboPostFX;			// Renderbuffer object for postprocess

	bool				m_bDrawDebugBuffers;		// toggle debug G-Buffer rendering!
	bool				m_bDrawBloomEffect;			// toggle Bloom effect!
	float				m_fBloomThreshold;			// Bloom cutoff value!

	GLSLShader*			m_pPostFXShader;			// Ptr to Postprocess shader
	GLSLShader*			m_pDeferredShader;			// Ptr to Deferred shader
	GLSLShader*			m_pDebugShader;				// Ptr to Debug shader

	// Postprocess properties
	float				m_fGamma;
	float				m_fExposure;
};

