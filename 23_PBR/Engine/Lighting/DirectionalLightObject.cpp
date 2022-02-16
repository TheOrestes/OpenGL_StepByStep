
#include "DirectionalLightObject.h"
#include "../Gameobjects/BBoxCube.h"
#include "../Main/Helper.h"

#include <vector>
#include "glm//gtc/epsilon.hpp"
#include "glm/gtc/matrix_access.hpp"

//////////////////////////////////////////////////////////////////////////////////////////
DirectionalLightObject::DirectionalLightObject()
{
	m_vecLightAngleXYZ = glm::vec3(0);
	m_vecLightDirection = glm::vec3(0);
	m_vecLightColor = glm::vec3(1);
	m_fIntensity = 1.0f;
	m_vecLightAngleXYZ = glm::vec3(0,0,0);

	m_matWorld = glm::mat4(1);
	m_matWorldToLightViewMatrix = glm::mat4(1.0f);
	m_matLightViewToProjectionMatrix = glm::mat4(1.0f);

	m_pDebugShadowVolume = nullptr;
	m_fShadowNearPlane = -25.0f;
	m_fShadowFarPlane = 50.0f;
	m_fShadowBounds = 35.0f;
	m_bShowDebugShadowVolume = false;

	Init();
}

//////////////////////////////////////////////////////////////////////////////////////////
DirectionalLightObject::DirectionalLightObject(const glm::vec3& color)
{
	m_vecLightAngleXYZ = glm::vec3(0, 0, 0);
	m_vecLightDirection = glm::vec3(0);
	m_vecLightColor = color;
	m_fIntensity = 2.0f;

	m_matWorld = glm::mat4(1);
	m_matWorldToLightViewMatrix = glm::mat4(1.0f);
	m_matLightViewToProjectionMatrix = glm::mat4(1.0f);

	m_pDebugShadowVolume = nullptr;
	m_fShadowNearPlane = 1.0f;
	m_fShadowFarPlane = 50.0f;
	m_fShadowBounds = 100.0f;
	m_bShowDebugShadowVolume = false;
	
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
	// glm::ortho(left, right, bottom, top, near, far)
	m_matLightViewToProjectionMatrix = glm::ortho(-m_fShadowBounds, m_fShadowBounds, -m_fShadowBounds, m_fShadowBounds, m_fShadowNearPlane, m_fShadowFarPlane);

	// Create vertex position values based on bounds, near & far plane! 
	std::vector<glm::vec3> volumeData;

	glm::vec3 pos0(-m_fShadowBounds, m_fShadowFarPlane, m_fShadowBounds);	
	glm::vec3 pos1(m_fShadowBounds,  m_fShadowFarPlane, m_fShadowBounds);	
	glm::vec3 pos2(m_fShadowBounds,  m_fShadowFarPlane, -m_fShadowBounds);
	glm::vec3 pos3(-m_fShadowBounds, m_fShadowFarPlane, -m_fShadowBounds);
	glm::vec3 pos4(-m_fShadowBounds, m_fShadowNearPlane, m_fShadowBounds);
	glm::vec3 pos5(m_fShadowBounds,  m_fShadowNearPlane, m_fShadowBounds);
	glm::vec3 pos6(m_fShadowBounds,  m_fShadowNearPlane, -m_fShadowBounds);
	glm::vec3 pos7(-m_fShadowBounds, m_fShadowNearPlane, -m_fShadowBounds);

	volumeData.push_back(pos0);
	volumeData.push_back(pos1);
	volumeData.push_back(pos2);
	volumeData.push_back(pos3);
	volumeData.push_back(pos4);
	volumeData.push_back(pos5);
	volumeData.push_back(pos6);
	volumeData.push_back(pos7);
													
	m_pDebugShadowVolume = new BBoxCube(volumeData);
	m_pDebugShadowVolume->Init();
}

//////////////////////////////////////////////////////////////////////////////////////////
void DirectionalLightObject::Kill()
{
	SAFE_DELETE(m_pDebugShadowVolume);
}

//////////////////////////////////////////////////////////////////////////////////////////
void DirectionalLightObject::Update( float dt )
{
	
}

//////////////////////////////////////////////////////////////////////////////////////////
void DirectionalLightObject::UpdateBounds()
{
	m_matLightViewToProjectionMatrix = glm::ortho(-m_fShadowBounds, m_fShadowBounds, 
												  -m_fShadowBounds, m_fShadowBounds, 
												   m_fShadowNearPlane, m_fShadowFarPlane);

	// Create vertex position values based on bounds, near & far plane! 
	std::vector<glm::vec3> volumeData;
	volumeData.reserve(8 * sizeof(glm::vec3));
	volumeData.clear();

	glm::vec3 pos0(-m_fShadowBounds, m_fShadowFarPlane, m_fShadowBounds);
	glm::vec3 pos1(m_fShadowBounds, m_fShadowFarPlane, m_fShadowBounds);
	glm::vec3 pos2(m_fShadowBounds, m_fShadowFarPlane, -m_fShadowBounds);
	glm::vec3 pos3(-m_fShadowBounds, m_fShadowFarPlane, -m_fShadowBounds);
	glm::vec3 pos4(-m_fShadowBounds, m_fShadowNearPlane, m_fShadowBounds);
	glm::vec3 pos5(m_fShadowBounds, m_fShadowNearPlane, m_fShadowBounds);
	glm::vec3 pos6(m_fShadowBounds, m_fShadowNearPlane, -m_fShadowBounds);
	glm::vec3 pos7(-m_fShadowBounds, m_fShadowNearPlane, -m_fShadowBounds);

	volumeData.push_back(pos0);
	volumeData.push_back(pos1);
	volumeData.push_back(pos2);
	volumeData.push_back(pos3);
	volumeData.push_back(pos4);
	volumeData.push_back(pos5);
	volumeData.push_back(pos6);
	volumeData.push_back(pos7);

	m_pDebugShadowVolume->UpdateBounds(volumeData);
}

//////////////////////////////////////////////////////////////////////////////////////////
void DirectionalLightObject::Render()
{
	if (!m_bShowDebugShadowVolume)
		return;

	m_pDebugShadowVolume->Render(m_matWorld);
}

//////////////////////////////////////////////////////////////////////////////////////////
void DirectionalLightObject::SetLightAngleXYZ(const glm::vec3& angleXYZ) 
{ 
	m_vecLightAngleXYZ = angleXYZ; 
	glm::mat4 rotateXYZ = glm::mat4(1);
	rotateXYZ = glm::rotate(rotateXYZ, glm::radians(m_vecLightAngleXYZ.z), glm::vec3(0, 0, 1));
	rotateXYZ = glm::rotate(rotateXYZ, glm::radians(m_vecLightAngleXYZ.y), glm::vec3(0, 1, 0));
	rotateXYZ = glm::rotate(rotateXYZ, glm::radians(m_vecLightAngleXYZ.x), glm::vec3(1, 0, 0));

	m_matWorld = rotateXYZ;

	m_vecLightDirection = glm::column(rotateXYZ, 1);

	// Set the light position far away in the opposite direction of light!
	// This is done since directional light has no position, but we need position to construct
	// the view matrix anyways!

	glm::vec3 lightPosition = -m_vecLightDirection * 20.0f;
	m_matWorldToLightViewMatrix = glm::lookAt(lightPosition, glm::vec3(0.0, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
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



