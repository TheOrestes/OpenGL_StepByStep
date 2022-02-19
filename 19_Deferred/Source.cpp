
#include <iostream>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "Camera.h"
#include "Application.h"
#include "Globals.h"

//////////////////////////////////////////////////////////////////////////
float tick = 0.016f;
bool  bFirstMouse = true;
float lastX = 400.0f;
float lastY = 300.0f;

//////////////////////////////////////////////////////////////////////////
// 1. Initialize GLFW & Create Window - Opengl context
GLFWwindow* InitGLFW(Application* pApp)
{
	if (!pApp)
	{
		std::cout << "Application pointer NULL";
		return nullptr;
	}

	// Initialize & Setup basic 
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	
	// Create a window!
	GLFWwindow* window = glfwCreateWindow(gWindowWidth, gWindowHeight, "Hello OpenGL Window", nullptr, nullptr);

	if (!window)
	{
		std::cout << "Create Window FAILED!!!\n";
		glfwTerminate();
		return nullptr;
	}

	// Window is created, now create context for the same window...
	glfwMakeContextCurrent(window);

	return window;
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
	GLFWwindow* window;
	Application* pApp = new Application();

	window = InitGLFW(pApp);
	InitGLEW();

	glfwSetKeyCallback(window, KeyHandler);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetCursorPosCallback(window, MouseHandler);

	pApp->Initialize();
	pApp->Run(window, 0.016f);

	glfwTerminate();

	delete pApp;

	return 0;
}