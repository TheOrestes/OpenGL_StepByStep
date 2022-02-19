
#include "Scene.h"
#include "StaticObject.h"
#include "PointLightObject.h"
#include "LightsManager.h"
#include "GLSkybox.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
Scene::Scene()
{
	m_pStaticObject1 = nullptr;
	m_pRedPointLight = nullptr;
	m_pGreenPointLight = nullptr;
	m_pBluePointLight = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Scene::~Scene()
{
	if (m_pStaticObject1)
	{
		delete m_pStaticObject1;
		m_pStaticObject1 = nullptr;
	}

	if (m_pRedPointLight)
	{
		delete m_pRedPointLight;
		m_pRedPointLight = nullptr;
	}

	if (m_pGreenPointLight)
	{
		delete m_pGreenPointLight;
		m_pGreenPointLight = nullptr;
	}

	if (m_pBluePointLight)
	{
		delete m_pBluePointLight;
		m_pBluePointLight = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::InitScene()
{
	// Initialize Static objects...
	StaticObjectData data;
	data.path = "../Assets/models/Barbarian/BarbNew.fbx";
	data.shader = "NormalMapWSDeferred";
	data.position = glm::vec3(0, 2, 0);
	data.angle = 0.0f;
	data.rotation = glm::vec3(0, 1, 0);
	data.scale = glm::vec3(1.0f);
	m_pStaticObject1 = new StaticObject(data);
	m_pStaticObject1->Init();

	// Load point lights
	m_pRedPointLight = new PointLightObject(glm::vec4(1, 0, 0, 1));
	m_pRedPointLight->SetLightIntensity(0.1f);
	m_pRedPointLight->SetLightPosition(glm::vec3(2, 5, 5));
	LightsManager::getInstance()->GatherPointLights(m_pRedPointLight);

	m_pGreenPointLight = new PointLightObject(glm::vec4(0, 1, 0, 1));
	m_pGreenPointLight->SetLightIntensity(0.1f);
	m_pGreenPointLight->SetLightPosition(glm::vec3(10, 15, 0));
	LightsManager::getInstance()->GatherPointLights(m_pGreenPointLight);

	m_pBluePointLight = new PointLightObject(glm::vec4(0, 0, 1, 1));
	m_pBluePointLight->SetLightIntensity(0.1f);
	m_pBluePointLight->SetLightPosition(glm::vec3(-5, 20, 5));
	LightsManager::getInstance()->GatherPointLights(m_pBluePointLight);

	// Load Skybox
	GLSkybox::getInstance().Init();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::Update(float dt)
{
	LightsManager::getInstance()->UpdateLights(dt);
	m_pStaticObject1->Update(dt);
	GLSkybox::getInstance().Update(dt);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::Render()
{
	GLSkybox::getInstance().Render();
	LightsManager::getInstance()->RenderLights();
	m_pStaticObject1->Render();
}