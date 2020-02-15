
#include "UIManager.h"
#include "../Main/Scene.h"
#include "../Lighting/LightsManager.h"
#include "../Lighting/DirectionalLightObject.h"
#include "../Lighting/PointLightObject.h"
#include "../Gameobjects/StaticObjectManager.h"
#include "../Gameobjects/StaticObject.h"
#include "../Gameobjects/Material.h"
#include "../Rendering/PostProcess.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UIManager::UIManager()
{
	m_bShowConsoleUI = false;
	m_bShowFPSUI = true;
	m_bShowSceneUI = true;
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
	if (!m_bShowSceneUI || !ptrScene || !ptrFX)
		return;

	ImGui::Begin("Scene Properties");

	if (ImGui::CollapsingHeader("Scene Objects"))
	{
		uint32_t objectCount = StaticObjectManager::getInstance().GetStaticObjectCount();

		for (uint32_t i = 0; i < objectCount; ++i)
		{
			StaticObject* object = StaticObjectManager::getInstance().GetStaticObjectAt(i);

			ImGui::PushID(i);
			ImGui::AlignFirstTextHeightToWidgets();

			if (ImGui::TreeNode(object->GetName().c_str()))
			{
				// File path
				ImGui::BulletText("FilePath"); ImGui::SameLine();
				ImGui::TextColored(ImVec4(0,1,0,1), object->GetPath().c_str());

				// Shader Name
				if (ImGui::TreeNode("Material"))
				{
					ImGui::BulletText("Shader"); ImGui::SameLine();
					ImGui::TextColored(ImVec4(0, 1, 0, 1), object->GetShaderName().c_str());

					Material* mat = object->GetMaterialPtr();
					
					float albedo[4] = { mat->m_colAlbedo.r, mat->m_colAlbedo.g, mat->m_colAlbedo.b, mat->m_colAlbedo.a };
					if(ImGui::ColorEdit4("Albedo Color", albedo))
						mat->m_colAlbedo = glm::vec4(albedo[0], albedo[1], albedo[2], albedo[3]);

					float roughness[4] = { mat->m_colRoughness.r, mat->m_colRoughness.g, mat->m_colRoughness.b, mat->m_colRoughness.a };
					if(ImGui::ColorEdit4("Roughness Color", roughness))
						mat->m_colRoughness = glm::vec4(roughness[0], roughness[1], roughness[2], roughness[3]);

					bool showWireframe = mat->m_bWireframe;
					if(ImGui::Checkbox("Wireframe", &showWireframe))
						mat->m_bWireframe = showWireframe;
				
					ImGui::TreePop();
				}


				if (ImGui::TreeNode("Transform"))
				{
					// Position
					glm::vec3 pos = object->GetPosition();
					
					if(ImGui::SliderFloat3("Position", glm::value_ptr(pos), -100, 100))
						object->SetPosition(pos);

					// Rotation Axis
					glm::vec3 rot = object->GetRotationAxis();
					if(ImGui::InputFloat3("Rotation Axis", glm::value_ptr(rot), 1))
						object->SetRotationAxis(rot);

					// Rotation Angle
					float rotAngle = object->GetCurrentAngle();
					if(ImGui::SliderAngle("Rotation Angle", &rotAngle))
						object->SetRotationAngle(rotAngle);

					// Scale
					glm::vec3 scale = object->GetScale();
					if(ImGui::InputFloat3("Scale", glm::value_ptr(scale)))
						object->SetScale(scale);

					// AutoUpdate
					bool autoRotate = object->GetAutoRotateFlag();
					if(ImGui::Checkbox("Auto Rotate", &autoRotate))
						object->SetAutoRotate(autoRotate);

					ImGui::TreePop();
				}
				
				ImGui::TreePop();
			}

			ImGui::PopID();
		}
	}

	if (ImGui::CollapsingHeader("Lighting"))
	{
		// Directional Lights!
		uint32_t dirLightCount = LightsManager::getInstance()->GetDirectionalLightsCount();
		for (uint32_t i = 0; i < dirLightCount; ++i)
		{
			ImGui::PushID(i);
			ImGui::AlignFirstTextHeightToWidgets();
		
			DirectionalLightObject* dirLightObject = LightsManager::getInstance()->GetDirectionalLight(i);
		
			std::string lightName = "DirectionalLight" + std::to_string(i);
			if (ImGui::TreeNode(lightName.c_str()))
			{
				glm::vec3	angleXYZ = dirLightObject->GetLightAngleXYZ();
				if (ImGui::SliderFloat3("Rotation", glm::value_ptr(angleXYZ), -360, 360, "%0.1f"))
					dirLightObject->SetLightAngleXYZ(angleXYZ);
		
				glm::vec3	color = dirLightObject->GetLightColor();
				if (ImGui::ColorEdit3("Color", glm::value_ptr(color)))
					dirLightObject->SetLightColor(color);
		
				float		intensity = dirLightObject->GetLightIntensity();
				if (ImGui::SliderFloat("Intensity", &intensity, 0, 10))
					dirLightObject->SetLightIntensity(intensity);

				ImGui::Separator();

				bool bShowShadowBounds = dirLightObject->IsShowDebugShadowVolumeOn();
				ImGui::Checkbox("Edit Shadow Bounds (Buggy, Crash!)", &bShowShadowBounds);
				dirLightObject->SetShowDebugShadowVolume(bShowShadowBounds);
				
				if (ImGui::CollapsingHeader("Shadow Bound Properties", &bShowShadowBounds))
				{
					float bound = dirLightObject->GetShadowBound();
					if (ImGui::SliderFloat("Bounds", &bound, 1.0f, 100.0f))
					{
						dirLightObject->SetShadowBound(bound);
						dirLightObject->UpdateBounds();
					}

					float nearPlane = dirLightObject->GetShadowNearPlane();
					if (ImGui::SliderFloat("Near Plane", &nearPlane, -50.0f, 100.0f))
					{
						dirLightObject->SetShadowNearPlane(nearPlane);
						dirLightObject->UpdateBounds();
					}

					float farPlane = dirLightObject->GetShadowFarPlane();
					if (ImGui::SliderFloat("Far Plane", &farPlane, 1.0f, 200.0f))
					{
						dirLightObject->SetShadowFarPlane(farPlane);
						dirLightObject->UpdateBounds();
					}
				}
					
				ImGui::TreePop();
			}

			ImGui::PopID();
		}
		

		// Point Lights!
		uint32_t pointLightCount = LightsManager::getInstance()->GetPointlightsCount();
		for (uint32_t j = 0; j < pointLightCount; ++j)
		{
			ImGui::PushID(j);
			ImGui::AlignFirstTextHeightToWidgets();
		
			PointLightObject* pointLightObject = LightsManager::getInstance()->GetPointLight(j);
		
			std::string lightName = "PointLight" + std::to_string(j);
			if (ImGui::TreeNode(lightName.c_str()))
			{
				glm::vec3	position = pointLightObject->GetLightPosition();
				if (ImGui::SliderFloat3("Position", glm::value_ptr(position), -100, 100))
					pointLightObject->SetLightPosition(position);
		
				glm::vec3	color = pointLightObject->GetLightColor();
				if (ImGui::ColorEdit3("Color", glm::value_ptr(color)))
					pointLightObject->SetLightColor(color);
		
				float		intensity = pointLightObject->GetLightIntensity();
				if (ImGui::SliderFloat("Intensity", &intensity, 0, 10))
					pointLightObject->SetLightIntensity(intensity);
				
				float		radius = pointLightObject->GetLightRadius();
				if (ImGui::SliderFloat("Radius", &radius, 0, 50))
					pointLightObject->SetLightRadius(radius);
		
				ImGui::TreePop();
			}

			ImGui::PopID();
		}
	}

	if (ImGui::CollapsingHeader("Postprocess"))
	{
		bool bDrawBloom = ptrFX->GetBloomStatus();
		if (ImGui::Checkbox("Bloom", &bDrawBloom))
		{
			ptrFX->SetBloomEffect(bDrawBloom);
		}

		float bloomCutoff = ptrFX->GetBloomThreshold();
		if(ImGui::SliderFloat("Bloom Cutoff", &bloomCutoff, 0.0f, 2.0f))
		{
			ptrFX->SetBloomThreshold(bloomCutoff);
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




