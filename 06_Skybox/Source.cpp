
#include <iostream>
#include "GL\glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "GLCube.h"
#include "Camera.h"
#include "GLSkybox.h"
#include "Globals.h"

//////////////////////////////////////////////////////////////////////////
GLFWwindow* window = nullptr;
float red = 0.1f;
float green = 0.1f;
float blue = 0.1f;

float tick					=	0.016f;
bool  bFirstMouse			=	true;
float lastX					=	static_cast<float>(gWindowWidth)/ 2.0f;
float lastY					=	static_cast<float>(gWindowHeight)/ 2.0f;

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
	window = glfwCreateWindow(static_cast<float>(gWindowWidth), static_cast<float>(gWindowHeight), "Hello OpenGL Window", nullptr, nullptr);

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

	// Application close!
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	// Camera controls..
	if (key == GLFW_KEY_W && (action == GLFW_REPEAT || GLFW_PRESS))
	{
		Camera::getInstance().ProcessKeyboard(CameraMovement::FORWARD, tick);	
	}

	if (key == GLFW_KEY_S && (action == GLFW_REPEAT || GLFW_PRESS))
	{
		Camera::getInstance().ProcessKeyboard(CameraMovement::BACK, tick);	
	}

	if (key == GLFW_KEY_A && (action == GLFW_REPEAT || GLFW_PRESS))
	{
		Camera::getInstance().ProcessKeyboard(CameraMovement::LEFT, tick);	
	}

	if (key == GLFW_KEY_D && (action == GLFW_REPEAT || GLFW_PRESS))
	{
		Camera::getInstance().ProcessKeyboard(CameraMovement::RIGHT, tick);	
	}
}

//////////////////////////////////////////////////////////////////////////
// Mouse Inputs
void MouseHandler(GLFWwindow* window, double xPos, double yPos)
{
	if(bFirstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		bFirstMouse = false;
	}

	GLfloat xoffset = xPos - lastX;
	GLfloat yoffset = lastY - yPos; 
	lastX = xPos;
	lastY = yPos;

	Camera::getInstance().ProcessMouseMovement(xoffset, yoffset);
}


int main()
{
	InitGLFW();
	InitGLEW();

	glfwSetKeyCallback(window, KeyHandler);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetCursorPosCallback(window, MouseHandler);
	

	//GLCube cube;  // random color cube
	GLCube cube(glm::vec4(1,1,0,1));
	cube.Init();

	// initialize skybox
	GLSkybox::getInstance().Init();

	// Message Loop!
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(red, green, blue, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT |  GL_DEPTH_BUFFER_BIT);

		cube.Update(tick);
		GLSkybox::getInstance().Update(tick);

		cube.Render();
		GLSkybox::getInstance().Render();


		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}