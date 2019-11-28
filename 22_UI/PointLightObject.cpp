
#include "PointLightObject.h"

//////////////////////////////////////////////////////////////////////////////////////////
PointLightObject::PointLightObject()
{
	m_vecLightPosition = glm::vec3(0);
	m_vecLightColor = glm::vec4(1);
	m_fIntensity = 1.0f;
	m_fRadius = 10.0f;

	Init();
}

//////////////////////////////////////////////////////////////////////////////////////////
PointLightObject::PointLightObject(const glm::vec4& color)
{
	m_vecLightPosition = glm::vec3(0);
	m_vecLightColor = color;
	m_fIntensity = 2.0f;
	m_fRadius = 50.0f;

	Init();
}

//////////////////////////////////////////////////////////////////////////////////////////
PointLightObject::~PointLightObject()
{
	Kill();	
}

//////////////////////////////////////////////////////////////////////////////////////////
void PointLightObject::Init()
{
	m_pLightMesh = new GLLight(m_vecLightColor);
	m_pLightMesh->Init();
}

//////////////////////////////////////////////////////////////////////////////////////////
void PointLightObject::Kill()
{
	delete m_pLightMesh;
}

//////////////////////////////////////////////////////////////////////////////////////////
void PointLightObject::Update( float dt )
{
	m_pLightMesh->Update(dt);
}

//////////////////////////////////////////////////////////////////////////////////////////
void PointLightObject::Render()
{
	m_pLightMesh->Render();
}

//////////////////////////////////////////////////////////////////////////////////////////
void PointLightObject::SetLightPosition(const glm::vec3& pos)
{ 
	m_vecLightPosition = pos; 
	m_pLightMesh->SetPosition(pos);
}

//////////////////////////////////////////////////////////////////////////////////////////
void PointLightObject::SetLightColor(const glm::vec4& col)
{ 
	m_vecLightColor = col; 

}

//////////////////////////////////////////////////////////////////////////////////////////
void PointLightObject::SetLightIntensity(float intensity)
{ 
	m_fIntensity = intensity; 
}

//////////////////////////////////////////////////////////////////////////////////////////
void PointLightObject::SetLightRadius(float radius)
{ 
	m_fRadius = radius; 
}

