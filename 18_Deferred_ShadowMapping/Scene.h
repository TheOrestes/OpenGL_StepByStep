#pragma once

#include "VertexStructures.h"

class StaticObject;
class PointLightObject;
class DirectionalLightObject;

class Scene
{
public:
	Scene();
	~Scene();

	void	InitScene();
	void	Update(float dt);
	void	Render();
	void	RenderShadowMap();

private:
	StaticObject*		m_pStaticObject1;
	StaticObject*		m_pShadowObject;
	StaticObject*		m_pStaticObject2;

	DirectionalLightObject* m_pMainDirectionalLight;
	PointLightObject*	m_pRedPointLight;
	PointLightObject*	m_pGreenPointLight;
	PointLightObject*	m_pBluePointLight;
};
