
#include "Scene.h"
#include "Application.h"
#include "PostProcess.h"
#include "Globals.h"
#include <iostream>

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
	m_pPostFX->Initialize();

	// Setup buffers for Deferred rendering
	m_pPostFX->CreateDeferredBuffers(gWindowWidth, gWindowHeight);

	// Setup buffers for Shadow mapping
	m_pPostFX->CreateShadowMappingBuffers(1024, 1024);

	// Setup buffers for Bloom
	m_pPostFX->CreateBloomBuffers(gWindowWidth, gWindowHeight);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Application::Run(GLFWwindow* window, float dt)
{
	double prevTime = glfwGetTime();
	int nFrames = 0;

	// Message Loop!
	while (!glfwWindowShouldClose(window))
	{
		double currTime = glfwGetTime();
		++nFrames;

		// check if one second has passed?
		if (currTime - prevTime >= 1.0f)
		{
			std::cout << "ms : " << 1000.0f / double(nFrames) << std::endl;
			std::cout << "FPS : " << nFrames << std::endl;
			nFrames = 0;
			prevTime += 1.0f;
		}

		glfwPollEvents();

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 1. Render Shadow map
		m_pPostFX->BeginShadowPass();
		m_pScene->Update(dt);
		m_pScene->RenderShadowMap();
		m_pPostFX->EndShadowPass();

		// 2. Render scene to Geometry Buffers!
		m_pPostFX->BeginGeometryRenderPass();
		m_pScene->Update(dt);
		m_pScene->Render();
		m_pPostFX->EndGeometryRenderPass();

		// 3. Perform Deferred rendering, but render to Bloom buffers!
		m_pPostFX->BeginBloomPrepass();
		m_pPostFX->ExecuteDeferredRenderPass();
		m_pPostFX->EndBloomPrepass();

		// 4. Perform Bloom
		m_pPostFX->ExecuteBloomPass();

		// 5. Debug Buffers
		// m_pPostFX->DrawDebugBuffers();

		glfwSwapBuffers(window);
	}
}