
#include <iostream>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "Gameobjects/Camera.h"
#include "Main/Application.h"
#include "Main/Globals.h"

//////////////////////////////////////////////////////////////////////////
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

	// Cap to the monitor's refresh rate (vsync) - an uncapped loop pins the GPU at 100% with no
	// visual benefit past the display's refresh rate, which is what was driving the boost/throttle
	// clock cycling (the "FPS swing") at this resolution.
	glfwSwapInterval(1);

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

	// Camera WASD movement is polled per-frame in Application::Run instead of handled here -
	// key-repeat events are throttled by the OS's auto-repeat delay/rate, decoupled from the render
	// loop, which made movement feel laggy/steppy next to the mouse's per-frame delta smoothness.
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

	// Rotate ONLY when RIGHT CLICK is down!
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		Camera::getInstance().ProcessMouseMovement(xoffset, yoffset);
	}
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

	pApp->Initialize(window);
	pApp->Run(0.016f);

	glfwTerminate();

	delete pApp;

	return 0;
}