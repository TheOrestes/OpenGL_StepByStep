
#include "Scene.h"
#include "Application.h"
#include "PostProcess.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
Application::Application()
{
	m_iWindowWidth = 1280;
	m_iWindowHeight = 800;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Application::~Application()
{
	if (m_pScene)
	{
		delete m_pScene;
		m_pScene = nullptr;
	}

	if (m_pPostFX)
	{
		delete m_pPostFX;
		m_pPostFX = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::Initialize()
{
	m_pScene = new Scene();
	m_pScene->InitScene();

	m_pPostFX = new PostProcess();
	m_pPostFX->SetupScreenQuad();

	// Setup buffers for Geometry pass!
	m_pPostFX->CreateColorDepthStencilBuffer(m_iWindowWidth, m_iWindowHeight);

	// Setup buffers for bloom (Color + Brightness)
	m_pPostFX->CreateColorBrightnessBuffer(m_iWindowWidth, m_iWindowHeight);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::Run(GLFWwindow* window, float dt)
{
	// Message Loop!
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_pPostFX->BeginRTPass();
		m_pScene->Update(dt);
		m_pScene->Render();
		m_pPostFX->EndRTPass();

		m_pPostFX->ExecuteBloomPass();

		glfwSwapBuffers(window);
	}
}