
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

	// Setup buffers for Deferred rendering
	m_pPostFX->CreateDeferredBuffers(gWindowWidth, gWindowHeight);

	// Setup buffers for Bloom
	m_pPostFX->CreateBloomBuffers(gWindowWidth, gWindowHeight);
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

		// 1. Render scene to Geometry Buffers!
		m_pPostFX->BeginGeometryRenderPass();
		m_pScene->Update(dt);
		m_pScene->Render();
		m_pPostFX->EndGeometryRenderPass();

		// 2. Perform Deferred rendering, but render to Bloom buffers!
		m_pPostFX->BeginBloomPrepass();
		m_pPostFX->ExecuteDeferredRenderPass();
		m_pPostFX->EndBloomPrepass();

		// 3. Perform Bloom
		m_pPostFX->ExecuteBloomPass();

		glfwSwapBuffers(window);
	}
}