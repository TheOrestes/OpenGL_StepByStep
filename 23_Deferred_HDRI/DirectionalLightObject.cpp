
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
	float fNearPlane = 1.0f;
	float fFarPlane = 50.0f;
	float fBounds = 100.0f;

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
	
	// Set the light position far away in the opposite direction of light!
	// This is done since directional light has no position, but we need position to construct
	// the view matrix anyways!

	glm::vec3 lightPosition = -m_vecLightDirection * 20.0f;
	m_matWorldToLightViewMatrix = glm::lookAt(lightPosition, glm::vec3(0.0, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
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



