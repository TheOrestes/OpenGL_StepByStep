
#include <iostream>
#include "GL\glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "GLCube.h"
#include "Globals.h"

//////////////////////////////////////////////////////////////////////////
GLFWwindow* window = nullptr;
float red = 0.1f;
float green = 0.1f;
float blue = 0.1f;

//////////////////////////////////////////////////////////////////////////
// 1. Initialize GLFW & Create Window - Opengl context
void InitGLFW()
{
	// Initialize & Setup basic 
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a window!
	window = glfwCreateWindow(gWindowWidth, gWindowHeight, "Hello OpenGL Window", nullptr, nullptr);

	if (!window)
	{
		std::cout << "Create Window FAILED!!!\n";
		glfwTerminate();
		return;
	}

	// Window is created, now create context for the same window...
	glfwMakeContextCurrent(window);
}

//////////////////////////////////////////////////////////////////////////
// 2. Initialize GLEW
void InitGLEW()
{
	// Ensure glew uses all the modern techniques...
	glewExperimental = GL_TRUE;

	// Initialize GLEW
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Initialize GLEW FAILED!!!\n";
		return;
	}
}

//////////////////////////////////////////////////////////////////////////
// 3. Inputs
void KeyHandler(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if(key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		red = 1.0f;
		green = blue = 0.0f;
	}

	if(key == GLFW_KEY_G && action == GLFW_PRESS)
	{
		green = 1.0f;
		red = blue = 0.0f;
	}

	if(key == GLFW_KEY_B && action == GLFW_PRESS)
	{
		blue = 1.0f;
		red = green = 0.0f;
	}
}


int main()
{
	InitGLFW();
	InitGLEW();

	glfwSetKeyCallback(window, KeyHandler);

	GLCube cube;  // random color cube
	cube.Init();

	// Message Loop!
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(red, green, blue, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT |  GL_DEPTH_BUFFER_BIT);

		cube.Update(0.016f);
		cube.Render();

		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}