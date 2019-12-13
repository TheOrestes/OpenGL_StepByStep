
#include "LightsManager.h"
#include "DirectionalLightObject.h"
#include "PointLightObject.h"
#include "UIManager.h"

//////////////////////////////////////////////////////////////////////////////////////////
LightsManager::LightsManager()
{
	m_iNumPointLights = 0;
	m_iNumDirLights = 0;

	m_vecDirLights.clear();
	m_vecPointLights.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
LightsManager::~LightsManager()
{

}

//////////////////////////////////////////////////////////////////////////////////////////
void LightsManager::GatherPointLights(PointLightObject* obj)
{
	m_vecPointLights.push_back(obj);
	m_iNumPointLights = m_vecPointLights.size();

	std::string msg = std::to_string(m_iNumDirLights) + " point light added";
	UIManager::getInstance().WriteToConsole(LOGTYPE::LOG_INFO, "Lights", msg);
}

//////////////////////////////////////////////////////////////////////////////////////////
PointLightObject* LightsManager::GetPointLight(int id)
{
	return m_vecPointLights.at(id);
}

//////////////////////////////////////////////////////////////////////////////////////////
void LightsManager::RenderLights()
{
	std::vector<PointLightObject*>::iterator iterPtLight = m_vecPointLights.begin();

	for ( ; iterPtLight != m_vecPointLights.end() ; ++iterPtLight )
	{
		(*iterPtLight)->Render();
	}

	std::vector<DirectionalLightObject*>::iterator iterDirLight = m_vecDirLights.begin();
	for (; iterDirLight != m_vecDirLights.end(); ++iterDirLight)
	{
		(*iterDirLight)->Render();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void LightsManager::UpdateLights( float dt )
{
	std::vector<PointLightObject*>::iterator iterPtLight = m_vecPointLights.begin();
	
	for ( ; iterPtLight != m_vecPointLights.end() ; ++iterPtLight )
	{
		(*iterPtLight)->Update(dt);
	}

	// similarly for Directional light
	std::vector<DirectionalLightObject*>::iterator iterDirLight = m_vecDirLights.begin();

	for (; iterDirLight != m_vecDirLights.end(); ++iterDirLight)
	{
		(*iterDirLight)->Update(dt);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void LightsManager::GatherDirectionalLights( DirectionalLightObject* obj )
{
	m_vecDirLights.push_back(obj);
	m_iNumDirLights = m_vecDirLights.size();

	UIManager::getInstance().WriteToConsole(LOGTYPE::LOG_INFO, "LightsManager", "Directional light added...");
}

////////////////////////////////////////////////////////////////////////////////////////
DirectionalLightObject* LightsManager::GetDirectionalLight( int id )
{
	return m_vecDirLights.at(id);
}