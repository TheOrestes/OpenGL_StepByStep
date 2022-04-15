
#pragma once

#include <iostream>
#include <time.h>
#include <vector>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "../../imgui/imgui.h"
#include "../../imgui/imgui_impl_glfw.h"
#include "../../imgui/imgui_impl_opengl3.h"

class Scene;
class PostProcess;

enum LOGTYPE 
{
	LOG_INFO = 0,
	LOG_DEBUG,
	LOG_ERROR,
	LOG_RAW,
	LOG_MAX
};

struct UIConsoleMsg
{
	UIConsoleMsg()
	{
		type = LOG_INFO;
		color = ImVec4(1, 1, 1, 1);
		msg.clear();
	}

	LOGTYPE		type;
	ImVec4		color;
	std::string msg;
};

class UIManager
{
public:
	static UIManager& getInstance()
	{
		static UIManager instance;
		return instance;
	}

	~UIManager();

	void	Initialize(GLFWwindow* window);
	void	WriteToConsole(LOGTYPE type, const std::string& file, const std::string& message);
	void	Render(Scene* ptrScene, PostProcess* pFX);
	void	Kill();

private:
	UIManager();
	std::vector<UIConsoleMsg> consoleMsgs;

	const std::string CurrentDateTime();

	void	RenderConsole();
	void	RenderFPS();
	void	RenderSceneUI(Scene* ptrScene, PostProcess* ptrFX);
	void	RenderMainMenuUI();

	//--- UI Variables
	bool	m_bShowConsoleUI;
	bool	m_bShowFPSUI;
	bool	m_bShowSceneUI;

	int		m_iChannelID;
};
