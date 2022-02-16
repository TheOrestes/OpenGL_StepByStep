#pragma once

#include "GLFW/glfw3.h"

class Scene;
class PostProcess;

class Application
{
public:
	Application();
	~Application();

	void	Initialize(GLFWwindow* window);
	void	Run(float dt);

	inline int	GetWindowWidth() const { return m_iWindowWidth; }
	inline int	GetWindowHeight() const { return m_iWindowHeight; }
	
private:

	int				m_iWindowWidth;
	int				m_iWindowHeight;

	GLFWwindow*		m_pWindow;
	Scene*			m_pScene;
	PostProcess*	m_pPostFX;
};
