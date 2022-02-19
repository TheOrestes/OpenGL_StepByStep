
#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "GLLight.h"

class PointLightObject
{
public:
	PointLightObject();
	PointLightObject(const glm::vec3& color);
	~PointLightObject();

	void		Init();
	void		Kill();
	void		Update(float dt);
	void		Render();

	// Getter
	inline glm::vec3	GetLightPosition(){ return m_vecLightPosition; }
	inline glm::vec3	GetLightColor(){ return m_vecLightColor; }
	inline float		GetLightIntensity(){ return m_fIntensity; }
	inline float		GetLightRadius(){ return m_fRadius; }

	// Setter
	void				SetLightPosition(const glm::vec3& pos);
	void				SetLightColor(const glm::vec3& col);
	void				SetLightIntensity(float intensity);
	void				SetLightRadius(float radius);

private:
	GLLight*			m_pLightMesh;

	glm::vec3			m_vecLightPosition;
	glm::vec3			m_vecLightColor;
	float				m_fIntensity;
	float				m_fRadius;
};
