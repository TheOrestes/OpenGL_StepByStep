
#include "UIManager.h"
#include "Scene.h"
#include "PointLightObject.h"
#include "PostProcess.h"
#include "Material.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UIManager::UIManager()
{
	m_bShowConsoleUI = false;
	m_bShowFPSUI = true;
	m_bShowSceneUI = false;
	m_bDrawBloom = true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UIManager::Initialize(GLFWwindow* window)
{
	// initialize imGUI...
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");
	
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("data/fonts/consola.ttf", 12.0f);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UIManager::WriteToConsole( LOGTYPE type, const std::string& file, const std::string& message )
{
	UIConsoleMsg msg;

	switch (type)
	{
		case LOG_INFO:
		{
			msg.color = ImVec4(0, 255, 0, 255);
			msg.msg = "INFO\t " + CurrentDateTime() + "\t" + file + "\t     " + message;
			msg.type = LOG_INFO;

			consoleMsgs.push_back(msg);
			break;
		}
			
		case LOG_ERROR:
		{
			msg.color = ImVec4(255, 0, 0, 255);
			msg.msg = "ERROR\t" + CurrentDateTime() + "\t" + file + "\t" + message;
			msg.type = LOG_ERROR;

			consoleMsgs.push_back(msg);		
			break;
		}
			
		case LOG_RAW:
		{
			msg.color = ImVec4(255, 255, 255, 255);
			msg.msg = message;
			msg.type = LOG_RAW;

			consoleMsgs.push_back(msg);
			break;
		}

		case LOG_DEBUG:
		{
			msg.color = ImVec4(255, 255, 0, 255);
			msg.msg = "DEBUG\t" + CurrentDateTime() + "\t" + file + "\t" + message;
			msg.type = LOG_DEBUG;

			consoleMsgs.push_back(msg);
			break;
		}

		default:
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// http://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c
const std::string UIManager::CurrentDateTime()
{
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];

	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d  %X", &tstruct);

	return buf;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UIManager::RenderConsole()
{	
	if (!m_bShowConsoleUI)
		return;

	ImGui::Begin("Console");

	static int option = 0;
	ImGui::RadioButton("Info",  &option, 0); ImGui::SameLine();
	ImGui::RadioButton("Debug", &option, 1); ImGui::SameLine();
	ImGui::RadioButton("Error", &option, 2); ImGui::SameLine();
	ImGui::RadioButton("Raw",   &option, 3); 

	std::vector<UIConsoleMsg>::iterator iter = consoleMsgs.begin();

	for (; iter != consoleMsgs.end(); iter++)
	{
		// Draw only from the option selected!
		if ((*iter).type == option)
		{
			ImGui::TextColored((*iter).color, (*iter).msg.c_str());
		}		
	}

	ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UIManager::RenderFPS()
{
	if (!m_bShowFPSUI)
		return;

	ImGui::Begin("DebugInfo");
	ImGui::Text("FPS : %f", ImGui::GetIO().Framerate);
	ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UIManager::RenderSceneUI(Scene* ptrScene, PostProcess* ptrFX)
{
	if (!m_bShowSceneUI)
		return;

	ImGui::Begin("Scene Properties");

	if (ImGui::CollapsingHeader("Scene Objects"))
	{

	}

	if (ImGui::CollapsingHeader("Lighting"))
	{

	}

	if (ImGui::CollapsingHeader("Shadows"))
	{

	}

	if (ImGui::CollapsingHeader("Postprocess"))
	{
		if (ImGui::Checkbox("Bloom", &m_bDrawBloom))
		{
			ptrFX->SetBloomEffect(m_bDrawBloom);
		}

		if(ImGui::SliderFloat("Bloom Cutoff", &m_fBloomCutoff, 0.5f, 2.0f))
		{
			
		}
	}

	if (ImGui::CollapsingHeader("Debug"))
	{
		if (ImGui::Checkbox("Draw G-Buffers", &m_bDrawGBuffers))
		{
			ptrFX->SetDebugDrawGBuffers(m_bDrawGBuffers);
		}
	}

	ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UIManager::RenderMainMenuUI()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			ImGui::MenuItem("Show FPS", nullptr, &m_bShowFPSUI);
			ImGui::MenuItem("Show Console", nullptr, &m_bShowConsoleUI);
			ImGui::MenuItem("Show Scene Menu", nullptr, &m_bShowSceneUI);
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	//Level* currLevel = LevelManager::getInstance().GetActiveLevel();
	//
	//ImGui::Begin("Level Objects");
	//
	//if (currLevel)
	//{
	//	std::vector<StaticObject*> sceneObjs = currLevel->GetLevelObjects();
	//	for (int i = 0; i < sceneObjs.size() ; i++)
	//	{
	//		StaticObject* obj = sceneObjs[i];
	//
	//		ImGui::PushID(i);
	//		ImGui::AlignFirstTextHeightToWidgets();
	//		bool node_open = ImGui::TreeNode("Object", "%s", obj->GetStaticObjectName().c_str());
	//
	//		if (node_open)
	//		{
	//			Material* mat = obj->GetMaterialPtr();
	//			glm::vec4 albedoColor = mat->getAlbedoColor();
	//			glm::vec4 roughnessColor = mat->getRoughnessColor();
	//
	//			ImGui::ColorEditMode(ImGuiColorEditMode_RGB);
	//			ImGui::PushID(0);
	//			ImGui::ColorEdit4("Albedo", glm::value_ptr(albedoColor));
	//			ImGui::ColorEdit4("Roughness", glm::value_ptr(roughnessColor));
	//			ImGui::PopID();
	//		
	//			ImGui::TreePop();
	//
	//			mat->setAlbedoColor( glm::value_ptr(albedoColor));
	//			mat->setRoughnessColor(glm::value_ptr(roughnessColor));
	//		}
	//		ImGui::PopID();
	//	}
	//}
	//
	//ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UIManager::Render(Scene* ptrScene, PostProcess* ptrFX)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	RenderConsole();
	RenderFPS();
	RenderSceneUI(ptrScene, ptrFX);
	RenderMainMenuUI();

	// Final Call!
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UIManager::Kill()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UIManager::~UIManager()
{

}




