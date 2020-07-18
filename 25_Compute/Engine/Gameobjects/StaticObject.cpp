
#include "StaticObject.h"
#include "Material.h"
#include "../Rendering/GLSLShader.h"
#include "../Rendering/Model.h"

//////////////////////////////////////////////////////////////////////////////////////////
StaticObject::StaticObject()
{
	m_strName.clear();
	m_strShaderName.clear();
	m_strPath.clear();

	m_pShader = nullptr;
	m_pShadowPassShader = nullptr;
	m_pModel = nullptr;
	m_pMaterial = nullptr;

	m_bAutoRotate = false;
	m_fAutoRotateSpeed = 0.1f;

	m_fCurrentRotationAngle = 0.0f;
}

//////////////////////////////////////////////////////////////////////////////////////////
StaticObject::StaticObject(const StaticObjectData& data)
	:
	m_strName(data.name),
	m_strPath(data.path),
	m_strShaderName(data.shader),
	m_pShader(nullptr),
	m_pShadowPassShader(nullptr),
	m_pModel(nullptr)
{
	m_vecPosition  = data.position;
	m_fAngle = data.angle;
	m_vecRotationAxis = data.rotation;
	m_vecScale = data.scale;

	m_bAutoRotate = data.autoRotate;
	m_fAutoRotateSpeed = 0.1f;

	m_fCurrentRotationAngle = 0.0f;
}

//////////////////////////////////////////////////////////////////////////////////////////
StaticObject::~StaticObject()
{
	Kill();
}

//////////////////////////////////////////////////////////////////////////////////////////
void StaticObject::Init()
{
	std::string vertShaderPath;
	vertShaderPath = "Shaders/" + m_strShaderName + ".vert";

	std::string fragShaderPath;
	fragShaderPath = "Shaders/" + m_strShaderName + ".frag";

	m_pShader = new GLSLShader(vertShaderPath, fragShaderPath);

	m_pShadowPassShader = new GLSLShader("Shaders/ShadowPass.vert", "Shaders/ShadowPass.frag");

	// initialize Material
	m_pMaterial = new Material();

	// initialize model
	m_pModel = new Model(m_strPath, m_pMaterial);
		
	// Initial transformation...
	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, m_vecPosition); 
	model = glm::rotate(model, m_fAngle, m_vecRotationAxis);
	model = glm::scale(model, m_vecScale);	

	m_matWorld = model;
}

//////////////////////////////////////////////////////////////////////////////////////////
void StaticObject::Kill()
{
	delete m_pShader;
	delete m_pModel;
	delete m_pMaterial;
}

//////////////////////////////////////////////////////////////////////////////////////////
void StaticObject::Update( float dt )
{
	if (m_bAutoRotate)
	{
		m_fCurrentRotationAngle += m_fAutoRotateSpeed * dt;
	}
		
	m_fAngle = m_fCurrentRotationAngle;

	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, m_vecPosition); 
	model = glm::rotate(model, m_fAngle, m_vecRotationAxis);
	model = glm::scale(model, m_vecScale);	

	m_matWorld = model;
}

//////////////////////////////////////////////////////////////////////////////////////////
void StaticObject::Render()
{
	m_pModel->Render(m_pShader, m_matWorld, m_pMaterial);
}

//////////////////////////////////////////////////////////////////////////////////////////
void StaticObject::RenderShadowMap()
{
	m_pModel->RenderShadowMap(m_pShadowPassShader, m_matWorld);
}






