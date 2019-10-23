
#include "DirectionalLightObject.h"


//////////////////////////////////////////////////////////////////////////////////////////
DirectionalLightObject::DirectionalLightObject()
{
	m_vecLightDirection = glm::vec3(0);
	m_vecLightColor = glm::vec4(1);
	m_fIntensity = 1.0f;

	m_matWorldToLightViewMatrix = glm::mat4(1.0f);
	m_matLightViewToProjectionMatrix = glm::mat4(1.0f);

	Init();
}

//////////////////////////////////////////////////////////////////////////////////////////
DirectionalLightObject::DirectionalLightObject(const glm::vec4& color)
{
	m_vecLightDirection = glm::vec3(0);
	m_vecLightColor = color;
	m_fIntensity = 2.0f;

	Init();
}

//////////////////////////////////////////////////////////////////////////////////////////
DirectionalLightObject::~DirectionalLightObject()
{
	Kill();	
}

//////////////////////////////////////////////////////////////////////////////////////////
void DirectionalLightObject::Init()
{
	float fNearPlane = -10.0f;
	float fFarPlane = 100.0f;
	float fBounds = 50.0f;

	m_matLightViewToProjectionMatrix = glm::ortho(-fBounds, fBounds, -fBounds, fBounds, fNearPlane, fFarPlane);
}

//////////////////////////////////////////////////////////////////////////////////////////
void DirectionalLightObject::Kill()
{

}

//////////////////////////////////////////////////////////////////////////////////////////
void DirectionalLightObject::Update( float dt )
{
	static float angle = 0;
	angle += 0.2f * dt;

	glm::mat4 rotMatrix(1);

	rotMatrix = glm::rotate(rotMatrix, angle, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec3 currLightDirection = glm::vec3(rotMatrix * glm::vec4(m_vecLightDirection, 1.0f));

	m_matWorldToLightViewMatrix = glm::lookAt(-currLightDirection, glm::vec3(0.0, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

//////////////////////////////////////////////////////////////////////////////////////////
void DirectionalLightObject::Render()
{

}

//////////////////////////////////////////////////////////////////////////////////////////
void DirectionalLightObject::SetLightDirection(const glm::vec3& dir)
{ 
	m_vecLightDirection = dir; 
}

//////////////////////////////////////////////////////////////////////////////////////////
void DirectionalLightObject::SetLightColor(const glm::vec4& col)
{ 
	m_vecLightColor = col; 

}

//////////////////////////////////////////////////////////////////////////////////////////
void DirectionalLightObject::SetLightIntensity(float intensity)
{ 
	m_fIntensity = intensity; 
}

/////////////////////////////////////////////////////////////////////////////////////////
void DirectionalLightObject::SetWorldToLightViewMatrix(const glm::mat4& _lightViewMatrix)
{
	m_matWorldToLightViewMatrix = _lightViewMatrix;
}

/////////////////////////////////////////////////////////////////////////////////////////
void DirectionalLightObject::SetLightViewToProjectionMatrix(const glm::mat4& _lightProjMatrix)
{
	m_matLightViewToProjectionMatrix = _lightProjMatrix;
}



