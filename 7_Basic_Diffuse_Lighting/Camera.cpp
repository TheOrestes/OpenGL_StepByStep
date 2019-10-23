
#include "Camera.h"

//////////////////////////////////////////////////////////////////////////////////////////
Camera::Camera() : 
	m_vecPosition(0,5,8),
	m_vecDirection(0,0,1),
	m_vecRight(1,0,0),
	m_vecUp(0,1,0),
	m_vecWorldUp(0,1,0),
	m_fYaw(-90.0f),
	m_fPitch(-5.0f),
	m_fSpeed(30.0f),
	m_fSensitivity(0.05f),
	m_fZoom(45.0f)
{
	Update();
}

//////////////////////////////////////////////////////////////////////////////////////////
Camera::~Camera()
{

}

//////////////////////////////////////////////////////////////////////////////////////////
void Camera::Update()
{
	// calculate new front vector based on updated yaw & pitch values...
	glm::vec3 front;
	front.x = cos(glm::radians(m_fYaw)) * cos(glm::radians(m_fPitch));
	front.y = sin(glm::radians(m_fPitch));
	front.z = sin(glm::radians(m_fYaw)) * cos(glm::radians(m_fPitch));

	// calculate new camera direction...
	m_vecDirection = glm::normalize(front);

	// calculate new right & up vector...
	m_vecRight = glm::normalize(glm::cross(m_vecDirection, m_vecWorldUp));
	m_vecUp = glm::normalize(glm::cross(m_vecRight, m_vecDirection));
}

//////////////////////////////////////////////////////////////////////////////////////////
void Camera::ProcessKeyboard(CameraMovement mov, float dt)
{
	float speed = m_fSpeed * dt;

	switch (mov)
	{
	case FORWARD:
		{
			m_vecPosition += m_vecDirection * speed;
		}
		break;

	case BACK:
		{
			m_vecPosition -= m_vecDirection * speed;
		}
		break;

	case LEFT:
		{
			m_vecPosition -= m_vecRight * speed;
		}
		break;

	case RIGHT:
		{
			m_vecPosition += m_vecRight * speed;
		}
		break;
	}

	// for FPS camera, enable this!
	// m_vecPosition.y = 0.0f;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Camera::ProcessMouseMovement(float xOffset, float yOffset, bool bConstraintPitch /* = true */)
{
	xOffset *= m_fSensitivity;
	yOffset *= m_fSensitivity;

	m_fYaw += xOffset;
	m_fPitch += yOffset;

	if(bConstraintPitch)
	{
		if(m_fPitch > 89.0f)
			m_fPitch = 89.0f;

		if (m_fPitch < -89.0f)
			m_fPitch = -89.0f;
	}

	Update();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Camera::ProcessMouseScroll(float offset)
{

}

//////////////////////////////////////////////////////////////////////////////////////////
glm::mat4x4 Camera::getViewMatrix()
{
	return glm::lookAt(m_vecPosition, m_vecPosition + m_vecDirection, m_vecUp);
}

//////////////////////////////////////////////////////////////////////////////////////////
glm::mat4x4 Camera::getProjectionMatrix()
{
	return glm::perspective(m_fZoom, 1.6f, 0.1f, 1000.0f);
}

