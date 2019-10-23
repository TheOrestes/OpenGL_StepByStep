
#pragma once

#include "GL/glew.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

enum CameraMovement
{
	FORWARD,
	BACK,
	LEFT,
	RIGHT
};

class Camera
{
public:
	static Camera& getInstance()
	{
		static Camera instance;
		return instance;
	}

	~Camera();

	void			Update();
	void			ProcessKeyboard(CameraMovement mov, float dt);
	void			ProcessMouseMovement(float xOffset, float yOffset, bool bConstraintPitch = true);
	void			ProcessMouseScroll(float offset);

	glm::mat4x4				getViewMatrix();
	glm::mat4x4				getProjectionMatrix();
	inline glm::vec3		getCameraPosition(){ return m_vecPosition; }
	inline glm::vec3		getCameraLookVector() { return m_vecDirection; }

private:
	Camera();

	Camera(const Camera&);
	void operator=(const Camera&);

	// Attributes
	glm::vec3		m_vecPosition;
	glm::vec3		m_vecDirection;
	glm::vec3		m_vecRight;
	glm::vec3		m_vecUp;

	glm::vec3		m_vecWorldUp;

	// Euler Angles
	float			m_fYaw;
	float			m_fPitch;

	// Options
	float			m_fSpeed;
	float			m_fSensitivity;
	float			m_fZoom;

	// Matrices
	glm::mat4		m_matView;
	glm::mat4		m_matProjection;
};