
#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "GLLight.h"

class DirectionalLightObject
{
public:
	DirectionalLightObject();
	DirectionalLightObject(const glm::vec4& color);
	~DirectionalLightObject();

	void		Init();
	void		Kill();
	void		Update(float dt);
	void		Render();

	// Getter
	inline glm::vec3	GetLightDirection(){ return m_vecLightDirection; }
	inline glm::vec4	GetLightColor(){ return m_vecLightColor; }
	inline float		GetLightIntensity(){ return m_fIntensity; }

	inline glm::mat4	GetWorldToLightViewMatrix() { return m_matWorldToLightViewMatrix; }
	inline glm::mat4	GetLightViewToProjectionMatrix() { return m_matLightViewToProjectionMatrix; }

	// Setter
	void				SetLightDirection(const glm::vec3& dir);
	void				SetLightColor(const glm::vec4& col);
	void				SetLightIntensity(float intensity);

	void				SetWorldToLightViewMatrix(const glm::mat4& _lightViewMatrix);
	void				SetLightViewToProjectionMatrix(const glm::mat4& _lightProjMatrix);

private:
	glm::vec3			m_vecLightDirection;
	glm::vec4			m_vecLightColor;
	float				m_fIntensity;

	glm::mat4			m_matWorldToLightViewMatrix;
	glm::mat4			m_matLightViewToProjectionMatrix;
};
