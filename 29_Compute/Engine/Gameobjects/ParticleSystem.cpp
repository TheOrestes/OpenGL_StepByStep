
#include "ParticleSystem.h"
#include "../Rendering/GLSLShader.h"
#include "../Gameobjects/Camera.h"
#include "../Main/Helper.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
ParticleSystem::ParticleSystem()
{
	m_uiParticleCount = 128000;
	m_vecParticles.reserve(m_uiParticleCount);

	m_matWorld = glm::mat4(1);
	m_fTotalTime = 0.0f;

	m_pRenderingShader = nullptr;
	m_pComputeShader = nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ParticleSystem::ParticleSystem(const uint64_t count)
{
	m_uiParticleCount = count;
	m_vecParticles.reserve(m_uiParticleCount);

	m_matWorld = glm::mat4(1);
	m_fTotalTime = 0.0f;

	m_pRenderingShader = nullptr;
	m_pComputeShader = nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ParticleSystem::~ParticleSystem()
{
	m_vecParticles.clear();

	SAFE_DELETE(m_pRenderingShader);
	SAFE_DELETE(m_pComputeShader);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ParticleSystem::InitializeParticleData()
{
	for (size_t i = 0; i < m_uiParticleCount; ++i)
	{
		Particle currParticle;
		
		currParticle.m_vecPosition = glm::vec4(	Helper::GetRandomRange(-10.0f, 10.0f), 
												0.0f, 
												Helper::GetRandomRange(-10.0f, 10.0f),
												1.0f);

		currParticle.m_vecVelocity = glm::vec4(0, 0, 0, 0);
		currParticle.m_vecAcceleration = glm::vec4(0, 0, 0, 0);

		glm::vec4 randomColor = glm::vec4(0);
		float random = rand() / (float)RAND_MAX;
		if (random < 0.33f)
		{
			randomColor = glm::vec4(rand() / (float)RAND_MAX, 0.0f, 0.0f, 1.0f);
		}
		else if (random > 0.33f && random < 0.67f)
		{
			randomColor = glm::vec4(0.0f, rand() / (float)RAND_MAX, 0.0f, 1.0f);
		}
		else
		{
			randomColor = glm::vec4(0.0f, 0.0f, rand() / (float)RAND_MAX, 1.0f);
		}

		currParticle.m_vecColor = randomColor;

		m_vecParticles.emplace_back(currParticle);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ParticleSystem::SetShaderVariables(int shaderID, const glm::mat4& world)
{
	//--- Transformation matrices
	glm::mat4 projection = Camera::getInstance().getProjectionMatrix();
	glm::mat4 view = Camera::getInstance().getViewMatrix();

	glUniformMatrix4fv(glGetUniformLocation(shaderID, "matProj"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "matView"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "matWorld"), 1, GL_FALSE, glm::value_ptr(world));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ParticleSystem::Initialize()
{
	// Load shaders...
	m_pRenderingShader = new GLSLShader("Shaders/Particle.vert", "Shaders/Particle.frag");
	m_pComputeShader = new GLSLShader("Shaders/Particle.comp");

	// Initialize Starting data for particle system!
	InitializeParticleData();

	// Intialize OpenGL Buffers!
	glGenVertexArrays(1, &m_vao);
	//glGenBuffers(1, &m_vbo);

	glGenBuffers(1, &m_ssboParticles);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssboParticles);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_uiParticleCount * sizeof(Particle), nullptr, GL_DYNAMIC_DRAW);

	GLint bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;

	Particle* particles = (Particle*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_uiParticleCount * sizeof(Particle), bufMask);
	particles = &m_vecParticles[0];
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_ssboParticles);
	glBufferData(GL_ARRAY_BUFFER, m_vecParticles.size() * sizeof(Particle), &m_vecParticles[0], GL_DYNAMIC_COPY);

	// particle position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)0);

	// particle velocity
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, m_vecVelocity));

	// particle acceleration
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, m_vecAcceleration));

	// Particle Color
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, m_vecColor));

	glBindVertexArray(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ParticleSystem::Update(float dt)
{
	m_fTotalTime += dt;
	// CPU based update!
	//glBindVertexArray(m_vao);
	//glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	//
	//for (size_t i = 0; i < m_vecParticles.size() ; ++i)
	//{
	//	m_vecParticles[i].m_vecPosition += glm::vec3(m_vecParticles[i].m_vecVelocity) * dt * 10.0f;
	//}
	//
	//glBufferSubData(GL_ARRAY_BUFFER, 0, m_vecParticles.size() * sizeof(Particle), &m_vecParticles[0]);
	//glBindVertexArray(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ParticleSystem::Render()
{
	m_pComputeShader->Use();
	glUniform1fv(glGetUniformLocation(m_pComputeShader->GetShaderID(), "totalTime"), 1, &m_fTotalTime);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_ssboParticles);
	glDispatchCompute(m_uiParticleCount / 128, 1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	int shaderID = m_pRenderingShader->GetShaderID();
	m_pRenderingShader->Use();

	SetShaderVariables(shaderID, m_matWorld);

	glEnable(GL_PROGRAM_POINT_SIZE);

	glBindVertexArray(m_vao);
	glDrawArrays(GL_POINTS, 0, m_vecParticles.size());
	glBindVertexArray(0);
}

