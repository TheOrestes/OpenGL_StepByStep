
#include "Scene.h"
#include "../Gameobjects/StaticObject.h"
#include "../Lighting/PointLightObject.h"
#include "../Lighting/DirectionalLightObject.h"
#include "../Lighting/LightsManager.h"
#include "../Gameobjects/StaticObjectManager.h"
#include "../Gameobjects/GLSkybox.h"
#include "../Gameobjects/HDRSkybox.h"
#include "../UI/UIManager.h"
#include "../Main/Helper.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
Scene::Scene()
{
	m_pRedPointLight = nullptr;
	m_pGreenPointLight = nullptr;
	m_pBluePointLight = nullptr;
	m_pMainDirectionalLight = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Scene::~Scene()
{
	SAFE_DELETE(m_pBluePointLight);
	SAFE_DELETE(m_pGreenPointLight);
	SAFE_DELETE(m_pMainDirectionalLight);
	SAFE_DELETE(m_pRedPointLight);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::InitScene()
{
	UIManager::getInstance().WriteToConsole(LOGTYPE::LOG_INFO, "Scene", "Scene loading started...");

	// Initialize Static objects...
	StaticObjectData data;
	data.name = "SteamPunk";
	data.path = "../Assets/models/Robot/SteamPunk.fbx";
	data.shader = "NormalMapWSDeferred";
	data.position = glm::vec3(4.8f, 0, 0);
	data.angle = 0.0f;
	data.rotation = glm::vec3(0, 1, 0);
	data.scale = glm::vec3(1.0f);
	StaticObject* objMesh = new StaticObject(data);

	StaticObjectManager::getInstance().GatherStaticObject(objMesh);

	StaticObjectData dataPiano;
	dataPiano.name = "Piano";
	dataPiano.path = "../Assets/models/Piano/Piano.fbx";
	dataPiano.shader = "NormalMapWSDeferred";
	dataPiano.position = glm::vec3(-9.5f, 9.5f, 2);
	dataPiano.angle = 57.0f;
	dataPiano.rotation = glm::vec3(1.1f, 0.7f, -0.4f);
	dataPiano.scale = glm::vec3(0.8f);
	StaticObject* pianoMesh = new StaticObject(dataPiano);

	StaticObjectManager::getInstance().GatherStaticObject(pianoMesh);

	// plane
	StaticObjectData dataPlane;
	dataPlane.name = "ShadowPlane";
	dataPlane.path = "../Assets/models/Planes/Plane_Bathroom.fbx";
	dataPlane.shader = "NormalMapWSDeferred";
	dataPlane.position = glm::vec3(0, 0, 0);
	dataPlane.angle = 0.0f;
	dataPlane.rotation = glm::vec3(0, 1, 0);
	dataPlane.scale = glm::vec3(10.0f);
	StaticObject* objShadow = new StaticObject(dataPlane);
	
	StaticObjectManager::getInstance().GatherStaticObject(objShadow);

	// Load point lights
	m_pRedPointLight = new PointLightObject(glm::vec4(1, 0, 0, 1));
	m_pRedPointLight->m_fLightIntensity = 0.15f;
	m_pRedPointLight->SetLightPosition(glm::vec3(2, 5, 5));
	//LightsManager::getInstance()->GatherPointLights(m_pRedPointLight);

	m_pGreenPointLight = new PointLightObject(glm::vec4(0, 1, 0, 1));
	m_pGreenPointLight->m_fLightIntensity = 0.15f;
	m_pGreenPointLight->SetLightPosition(glm::vec3(10, 15, 0));
	//LightsManager::getInstance()->GatherPointLights(m_pGreenPointLight);

	m_pBluePointLight = new PointLightObject(glm::vec4(0, 0, 1, 1));
	m_pBluePointLight->m_fLightIntensity = 0.15f;
	m_pBluePointLight->SetLightPosition(glm::vec3(-5, 20, 5));
	//LightsManager::getInstance()->GatherPointLights(m_pBluePointLight);

	// Load directional lights
	m_pMainDirectionalLight = new DirectionalLightObject(glm::vec4(1, 1, 1, 1));
	m_pMainDirectionalLight->SetEulerLightAngles(glm::vec3(-155.0f, 0, 0));
	m_pMainDirectionalLight->m_fLightIntensity = 3.0f;
	LightsManager::getInstance()->GatherDirectionalLights(m_pMainDirectionalLight);

	// Load Skybox
	//GLSkybox::getInstance().Init();

	// Load HDRI
	HDRSkybox::getInstance().Initialize();

	UIManager::getInstance().WriteToConsole(LOGTYPE::LOG_INFO, "Scene", "Scene loading ended...");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::Update(float dt)
{
	LightsManager::getInstance()->UpdateLights(dt);
	StaticObjectManager::getInstance().UpdateAll(dt);
	HDRSkybox::getInstance().Update(dt);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::Render()
{
	//GLSkybox::getInstance().Render();
	LightsManager::getInstance()->RenderLights();
	StaticObjectManager::getInstance().RenderAll();
	HDRSkybox::getInstance().Render();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::RenderShadowMap()
{
	StaticObjectManager::getInstance().RenderAllToShadowmap();
}