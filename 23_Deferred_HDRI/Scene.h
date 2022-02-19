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

	DirectionalLightObject* m_pMainDirectionalLight;
};
