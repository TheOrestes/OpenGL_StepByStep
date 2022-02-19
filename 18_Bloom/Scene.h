#pragma once

#include "VertexStructures.h"

class StaticObject;
class PointLightObject;

class Scene
{
public:
	Scene();
	~Scene();

	void	InitScene();
	void	Update(float dt);
	void	Render();

private:
	StaticObject*		m_pStaticObject1;
	PointLightObject*	m_pRedPointLight;
	PointLightObject*	m_pGreenPointLight;
	PointLightObject*	m_pBluePointLight;
};
