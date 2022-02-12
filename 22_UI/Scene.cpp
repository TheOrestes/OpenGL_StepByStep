
#include "Scene.h"
#include "StaticObject.h"
#include "PointLightObject.h"
#include "DirectionalLightObject.h"
#include "LightsManager.h"
#include "StaticObjectManager.h"
#include "GLSkybox.h"
#include "HDRSkybox.h"
#include "UIManager.h"
#include "Helper.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
Scene::Scene()
{
	m_pMainDirectionalLight = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Scene::~Scene()
{
	SAFE_DELETE(m_pMainDirectionalLight);
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
	data.position = glm::vec3(0, 2, 0);
	data.angle = 0.0f;
	data.rotation = glm::vec3(0, 1, 0);
	data.scale = glm::vec3(1.0f);
	StaticObject* objMesh = new StaticObject(data);

	StaticObjectManager::getInstance().GatherStaticObject(objMesh);

	// plane
	StaticObjectData dataPlane;
	dataPlane.name = "ShadowPlane";
	dataPlane.path = "../Assets/models/Planes/Plane_Oak.fbx";
	dataPlane.shader = "NormalMapWSDeferred";
	dataPlane.position = glm::vec3(0, 0, 0);
	dataPlane.angle = 0.0f;
	dataPlane.rotation = glm::vec3(0, 1, 0);
	dataPlane.scale = glm::vec3(20.0f);
	StaticObject* objShadow = new StaticObject(dataPlane);
	
	StaticObjectManager::getInstance().GatherStaticObject(objShadow);

	// Load directional lights
	m_pMainDirectionalLight = new DirectionalLightObject(glm::vec4(1, 1, 1, 1));
	m_pMainDirectionalLight->SetLightDirection(glm::vec3(0.5f, -1, -0.707f));
	m_pMainDirectionalLight->SetLightIntensity(0.1f);
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
	HDRSkybox::getInstance().Render();
	LightsManager::getInstance()->RenderLights();
	StaticObjectManager::getInstance().RenderAll();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::RenderShadowMap()
{
	StaticObjectManager::getInstance().RenderAllToShadowmap();
}
