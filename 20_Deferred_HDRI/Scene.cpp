
#include "Scene.h"
#include "StaticObject.h"
#include "PointLightObject.h"
#include "DirectionalLightObject.h"
#include "LightsManager.h"
#include "GLSkybox.h"
#include "HDRSkybox.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
Scene::Scene()
{
	m_pStaticObject1 = nullptr;
	m_pRedPointLight = nullptr;
	m_pGreenPointLight = nullptr;
	m_pBluePointLight = nullptr;
	m_pMainDirectionalLight = nullptr;
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

	if (m_pMainDirectionalLight)
	{
		delete m_pMainDirectionalLight;
		m_pMainDirectionalLight = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::InitScene()
{
	// Initialize Static objects...
	StaticObjectData data;
	data.path = "../Assets/models/Spaceship.fbx";
	data.shader = "NormalMapWSDeferred";
	data.position = glm::vec3(0, 5, 0);
	data.angle = 0.0f;
	data.rotation = glm::vec3(0, 1, 0);
	data.scale = glm::vec3(1.5f);
	m_pStaticObject1 = new StaticObject(data);
	m_pStaticObject1->Init();

	// plane
	StaticObjectData dataPlane;
	dataPlane.path = "../Assets/models/ShadowPlane.fbx";
	dataPlane.shader = "NormalMapWSDeferred";
	dataPlane.position = glm::vec3(0, 0, 0);
	dataPlane.angle = 0.0f;
	dataPlane.rotation = glm::vec3(0, 1, 0);
	dataPlane.scale = glm::vec3(10.0f);
	m_pShadowObject = new StaticObject(dataPlane);
	m_pShadowObject->Init();

	// Load point lights
	m_pRedPointLight = new PointLightObject(glm::vec4(1, 0, 0, 1));
	m_pRedPointLight->SetLightIntensity(0.15f);
	m_pRedPointLight->SetLightPosition(glm::vec3(2, 5, 5));
	LightsManager::getInstance()->GatherPointLights(m_pRedPointLight);

	m_pGreenPointLight = new PointLightObject(glm::vec4(0, 1, 0, 1));
	m_pGreenPointLight->SetLightIntensity(0.15f);
	m_pGreenPointLight->SetLightPosition(glm::vec3(10, 15, 0));
	LightsManager::getInstance()->GatherPointLights(m_pGreenPointLight);

	m_pBluePointLight = new PointLightObject(glm::vec4(0, 0, 1, 1));
	m_pBluePointLight->SetLightIntensity(0.15f);
	m_pBluePointLight->SetLightPosition(glm::vec3(-5, 20, 5));
	LightsManager::getInstance()->GatherPointLights(m_pBluePointLight);

	// Load directional lights
	m_pMainDirectionalLight = new DirectionalLightObject(glm::vec4(1, 1, 1, 1));
	m_pMainDirectionalLight->SetLightDirection(glm::vec3(1, -1, -1));
	m_pMainDirectionalLight->SetLightIntensity(0.25f);
	LightsManager::getInstance()->GatherDirectionalLights(m_pMainDirectionalLight);

	// Load Skybox
	//GLSkybox::getInstance().Init();

	// Load HDRI
	HDRSkybox::getInstance().Initialize();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::Update(float dt)
{
	LightsManager::getInstance()->UpdateLights(dt);
	m_pStaticObject1->Update(dt);
	HDRSkybox::getInstance().Update(dt);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::Render()
{
	//GLSkybox::getInstance().Render();
	HDRSkybox::getInstance().Render();
	LightsManager::getInstance()->RenderLights();
	m_pStaticObject1->Render();
	m_pShadowObject->Render();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::RenderShadowMap()
{
	m_pStaticObject1->RenderShadowMap();
	m_pShadowObject->RenderShadowMap();
}