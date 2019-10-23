
#include <iostream>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "GLCube.h"
#include "Camera.h"
#include "GLSkybox.h"
#include "StaticObject.h"
#include "PointLightObject.h"
#include "LightsManager.h"

//////////////////////////////////////////////////////////////////////////
GLFWwindow* window = nullptr;
float red = 0.1f;
float green = 0.1f;
float blue = 0.1f;

const int gScreenWidth		=	1280;
const int gScreenHeight		=	800;
float tick					=	0.016f;
bool  bFirstMouse			=	true;
float lastX					=	gScreenWidth / 2.0f;
float lastY					=	gScreenHeight / 2.0f;

//////////////////////////////////////////////////////////////////////////
// 1. Initialize GLFW & Create Window - OpenGL context
void InitGLFW()
{
	// Initialize & Setup basic 
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a window!
	window = glfwCreateWindow(gScreenWidth, gScreenHeight, "Hello OpenGL Window", nullptr, nullptr);

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
	
	StaticObjectData data;
	data.path = "../Assets/models/Anvil.fbx";
	data.shader = "NormalMapTS";
	data.position = glm::vec3(0,2,0);
	data.angle = 0.0f;
	data.rotation = glm::vec3(0,1,0);                                        
	data.scale = glm::vec3(0.2);
	StaticObject* obj1 = new StaticObject(data);
	obj1->Init();

	// Load point lights
	PointLightObject ptRedLight(glm::vec4(1, 0, 0, 1));
	ptRedLight.SetLightPosition(glm::vec3(2, 5, 5));
	LightsManager::getInstance()->GatherPointLights(&ptRedLight);

	PointLightObject ptGreenLight(glm::vec4(0, 1, 0, 1));
	ptGreenLight.SetLightPosition(glm::vec3(10, 15, 0));
	LightsManager::getInstance()->GatherPointLights(&ptGreenLight);

	PointLightObject ptBlueLight(glm::vec4(1, 1, 1, 1));
	ptBlueLight.SetLightPosition(glm::vec3(-5, 20, 5));
	ptBlueLight.SetLightIntensity(3.0f);
	LightsManager::getInstance()->GatherPointLights(&ptBlueLight);

	// initialize skybox
	GLSkybox::getInstance().Init();

	// Message Loop!
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(red, green, blue, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT |  GL_DEPTH_BUFFER_BIT);

		LightsManager::getInstance()->UpdateLights(tick);
		obj1->Update(tick);
		GLSkybox::getInstance().Update(tick);

		LightsManager::getInstance()->RenderLights();
		obj1->Render();
		GLSkybox::getInstance().Render();


		glfwSwapBuffers(window);
	}

	delete obj1;
	glfwTerminate();

	return 0;
}