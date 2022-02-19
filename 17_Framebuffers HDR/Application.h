#pragma once

#include "GLFW/glfw3.h"

class Scene;
class PostProcess;

class Application
{
public:
	Application();
	~Application();

	void	Initialize();
	void	Run(GLFWwindow* window, float dt);

	inline int	GetWindowWidth() { return m_iWindowWidth; }
	inline int	GetWindowHeight() { return m_iWindowHeight; }
	
private:

	int				m_iWindowWidth;
	int				m_iWindowHeight;

	Scene*			m_pScene;
	PostProcess*	m_pPostFX;
};
