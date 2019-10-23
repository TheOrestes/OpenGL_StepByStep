
#include "StaticObject.h"
#include "GLSLShader.h"
#include "Model.h"
#include "Material.h"

//////////////////////////////////////////////////////////////////////////////////////////
StaticObject::StaticObject()
{
	m_strShader.clear();
	m_strPath.clear();

	m_pShader = nullptr;
	m_pModel = nullptr;
	m_pMaterial = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
StaticObject::StaticObject(const StaticObjectData& data)
	:	m_strPath(data.path),
		m_strShader(data.shader),
		m_pShader(nullptr),
		m_pModel(nullptr)
{
	m_vecPosition  = data.position;
	m_fAngle = data.angle;
	m_vecRotation = data.rotation;
	m_vecScale = data.scale; 
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
	fragShaderPath = "../Assets/Shaders/ps" + m_strShader + ".glsl";

	m_pShader = new GLSLShader(vertShaderPath, fragShaderPath);

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
	static float angle = 0;
	angle += dt;

	m_fAngle = angle;

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
void StaticObject::SetPosition( const glm::vec3& pos )
{
	m_vecPosition = pos;
}

//////////////////////////////////////////////////////////////////////////////////////////
void StaticObject::SetRotation( const glm::vec3& axis, float angle )
{
	m_vecRotation = axis;
	m_fAngle = angle;
}

//////////////////////////////////////////////////////////////////////////////////////////
void StaticObject::SetScale( const glm::vec3& sc )
{
	m_vecScale = sc;
}





