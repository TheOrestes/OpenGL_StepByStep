
#include "LightsManager.h"
//#include "DirectionalLightObject.h"
#include "PointLightObject.h"

//////////////////////////////////////////////////////////////////////////////////////////
LightsManager::LightsManager()
{
	m_iNumPointLights = 0;
	m_iNumDirLights = 0;

	//m_vecDirLights.clear();
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

	//std::vector<DirectionalLightObject*>::iterator iterDirLight = m_vecDirLights.begin();
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
}

//////////////////////////////////////////////////////////////////////////////////////////
//void LightsManager::GatherDirectionalLights( DirectionalLightObject* obj )
//{
//	m_vecDirLights.push_back(obj);
//	m_iNumDirLights = m_vecDirLights.size();
//}

//////////////////////////////////////////////////////////////////////////////////////////
//DirectionalLightObject* LightsManager::GetDirectionalLight( int id )
//{
//	return m_vecDirLights.at(id);
//}