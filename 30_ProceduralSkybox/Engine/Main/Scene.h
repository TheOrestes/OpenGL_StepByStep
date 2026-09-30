#pragma once

#include "../Rendering/VertexStructures.h"

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
	PointLightObject*	m_pRedPointLight;
	PointLightObject*	m_pGreenPointLight;
	PointLightObject*	m_pBluePointLight;
};
