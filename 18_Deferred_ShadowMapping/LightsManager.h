
#pragma once

#include <vector>

class DirectionalLightObject;
class PointLightObject;

class LightsManager
{
public:
	static LightsManager* getInstance()
	{
		static LightsManager instance;
		return &instance;
	}

	~LightsManager();

	void							RenderLights();
	void							UpdateLights(float dt);

	void							GatherDirectionalLights(DirectionalLightObject* obj);
	void							GatherPointLights(PointLightObject* obj);
	
	DirectionalLightObject*			GetDirectionalLight(int id);
	PointLightObject*				GetPointLight(int id);

	inline	int						GetPointlightsCount(){ return m_iNumPointLights; }
	inline	int						GetDirectionalLightsCount(){ return m_iNumDirLights; }

private:
	LightsManager();

	int								m_iNumDirLights;
	int								m_iNumPointLights;

	std::vector<DirectionalLightObject*>	m_vecDirLights;
	std::vector<PointLightObject*>			m_vecPointLights;	
};