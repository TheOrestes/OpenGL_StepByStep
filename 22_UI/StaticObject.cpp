
#include "StaticObject.h"
#include "GLSLShader.h"
#include "Model.h"
#include "Material.h"

//////////////////////////////////////////////////////////////////////////////////////////
StaticObject::StaticObject()
{
	m_strName.clear();
	m_strShader.clear();
	m_strPath.clear();

	m_pShader = nullptr;
	m_pShadowPassShader = nullptr;
	m_pModel = nullptr;
	m_pMaterial = nullptr;

	m_bAutoRotate = false;

	m_fCurrentAngle = 0.0f;
}

//////////////////////////////////////////////////////////////////////////////////////////
StaticObject::StaticObject(const StaticObjectData& data)
	:
	m_strName(data.name),
	m_strPath(data.path),
	m_strShader(data.shader),
	m_pShader(nullptr),
	m_pShadowPassShader(nullptr),
	m_pModel(nullptr)
{
	m_vecPosition  = data.position;
	m_fAngle = data.angle;
	m_vecRotation = data.rotation;
	m_vecScale = data.scale;

	m_bAutoRotate = data.autoRotate;

	m_fCurrentAngle = 0.0f;
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
	vertShaderPath = "Shaders/vs" + m_strShader + ".glsl";

	std::string fragShaderPath;
	fragShaderPath = "Shaders/ps" + m_strShader + ".glsl";

	m_pShader = new GLSLShader(vertShaderPath, fragShaderPath);

	m_pShadowPassShader = new GLSLShader("Shaders/vsShadowPass.glsl", "Shaders/psShadowPass.glsl");

	// initialize Material
	m_pMaterial = new Material();

	// initialize model
	m_pModel = new Model(m_strPath, m_pMaterial);
		
	// Initial transformation...
	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, m_vecPosition); 
	model = glm::rotate(model, m_fAngle, m_vecRotation);
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
		m_fCurrentAngle += 0.2f * dt;
	}
		
	m_fAngle = m_fCurrentAngle;

	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, m_vecPosition); 
	model = glm::rotate(model, m_fAngle, m_vecRotation);
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






