
#include "Scene.h"
#include "Application.h"
#include "PostProcess.h"
#include "Globals.h"

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

	// Setup buffers for geometry pass!
	m_pPostFX->CreateColorDepthStencilBuffer(gWindowWidth, gWindowHeight);

	// Setup buffers for Deferred rendering
	m_pPostFX->CreateDeferredBuffers(gWindowWidth, gWindowHeight);
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

		m_pPostFX->BeginGeometryRenderPass();
		m_pScene->Update(dt);
		m_pScene->Render();
		m_pPostFX->EndGeometryRenderPass();

		m_pPostFX->ExecuteDeferredRenderPass();

		glfwSwapBuffers(window);
	}
}