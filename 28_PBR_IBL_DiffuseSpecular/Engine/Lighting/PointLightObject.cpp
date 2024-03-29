
#include "PointLightObject.h"

//////////////////////////////////////////////////////////////////////////////////////////
PointLightObject::PointLightObject()
{
	m_vecLightPosition = glm::vec3(0);
	m_vecLightColor = glm::vec3(1);
	m_fLightIntensity = 1.0f;
	m_fLightRadius = 5.0f;

	Init();
}

//////////////////////////////////////////////////////////////////////////////////////////
PointLightObject::PointLightObject(const glm::vec3& color)
{
	m_vecLightPosition = glm::vec3(0);
	m_vecLightColor = color;
	m_fLightIntensity = 2.0f;
	m_fLightRadius = 5.0f;

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
void PointLightObject::SetLightColor(const glm::vec3& col)
{ 
	m_vecLightColor = col; 
	m_pLightMesh->SetColor(col);
}


