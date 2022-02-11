
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
	data.path = "../Assets/models/Robot/SteamPunk.fbx";
	data.shader = "NormalMapWSDeferred";
	data.position = glm::vec3(0, 2, 0);
	data.angle = 0.0f;
	data.rotation = glm::vec3(0, 1, 0);
	data.scale = glm::vec3(1.0f);
	m_pStaticObject1 = new StaticObject(data);
	m_pStaticObject1->Init();

	// plane
	StaticObjectData dataPlane;
	dataPlane.path = "../Assets/models/Planes/Plane_Oak.fbx";
	dataPlane.shader = "NormalMapWSDeferred";
	dataPlane.position = glm::vec3(0, 0, 0);
	dataPlane.angle = 0.0f;
	dataPlane.rotation = glm::vec3(0, 1, 0);
	dataPlane.scale = glm::vec3(20.0f);
	m_pShadowObject = new StaticObject(dataPlane);
	m_pShadowObject->Init();

	// Load directional lights
	m_pMainDirectionalLight = new DirectionalLightObject(glm::vec4(1, 1, 1, 1));
	m_pMainDirectionalLight->SetLightDirection(glm::vec3(0.5f, -1, -0.707f));
	m_pMainDirectionalLight->SetLightIntensity(0.1f);
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
