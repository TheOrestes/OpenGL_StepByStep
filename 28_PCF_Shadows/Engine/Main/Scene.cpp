
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
	data.name = "Mannequin1";
	data.path = "../Assets/models/Spheres/Sphere_Wood.fbx";
	data.shader = "NormalMapWSDeferred";
	data.position = glm::vec3(4.0f, 0, 0);
	data.angle = 0.0f;
	data.rotation = glm::vec3(0, 1, 0);
	data.scale = glm::vec3(1.f);
	StaticObject* mesh1 = new StaticObject(data);

	StaticObjectManager::getInstance().GatherStaticObject(mesh1);

	StaticObjectData data2;
	data2.name = "Mannequin2";
	data2.path = "../Assets/models/Spheres/Sphere_RustedIron.fbx";
	data2.shader = "NormalMapWSDeferred";
	data2.position = glm::vec3(0.f, 0.f, 0.f);
	data2.angle = 0.0f;
	data2.rotation = glm::vec3(0,1,0);
	data2.scale = glm::vec3(1.f);
	StaticObject* mesh2 = new StaticObject(data2);

	StaticObjectManager::getInstance().GatherStaticObject(mesh2);

	StaticObjectData data3;
	data3.name = "Mannequin2";
	data3.path = "../Assets/models/Robot/SteamPunk.fbx";
	data3.shader = "NormalMapWSDeferred";
	data3.position = glm::vec3(0.f, 0.f, 0.f);
	data3.angle = 0.0f;
	data3.rotation = glm::vec3(0, 1, 0);
	data3.scale = glm::vec3(1.f);
	StaticObject* mesh3 = new StaticObject(data3);

	StaticObjectManager::getInstance().GatherStaticObject(mesh3);

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