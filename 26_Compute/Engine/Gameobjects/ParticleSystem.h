#pragma once

#include "glm/glm.hpp"
#include "../Rendering/VertexStructures.h"
#include <vector>

class GLSLShader;

class ParticleSystem
{
public:
	ParticleSystem();
	ParticleSystem(const uint64_t count);
	~ParticleSystem();

	void			Initialize();
	void			Update(float dt);
	void			Render();

private:

	void			InitializeParticleData();
	void			SetShaderVariables(int shaderID, const glm::mat4& world);

	GLSLShader*		m_pRenderingShader;
	GLSLShader*		m_pComputeShader;

	GLuint			m_vao;
	GLuint			m_vbo;

	uint64_t		m_uiParticleCount;

	glm::mat4		m_matWorld;

	std::vector<Particle>	m_vecParticles;

	GLuint			m_ssboParticles;
	float			m_fTotalTime;
};
 