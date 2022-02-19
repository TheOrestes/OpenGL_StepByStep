#pragma once

#include "VertexStructures.h"

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

	DirectionalLightObject* m_pMainDirectionalLight;
};
