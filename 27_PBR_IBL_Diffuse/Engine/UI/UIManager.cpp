
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

	m_iChannelID = 0;
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
	io.Fonts->AddFontFromFileTTF("../Assets/Fonts/consola.ttf", 13.0f);
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

			if (ImGui::TreeNode(object->m_strName.c_str()))
			{
				// File path
				ImGui::BulletText("FilePath"); ImGui::SameLine();
				ImGui::TextColored(ImVec4(0,1,0,1), object->m_strPath.c_str());

				// Shader Name
				if (ImGui::TreeNode("Material"))
				{
					ImGui::BulletText("Shader"); ImGui::SameLine();
					ImGui::TextColored(ImVec4(0, 1, 0, 1), object->m_strShaderName.c_str());

					Material* mat = object->GetMaterialPtr();
					
					float albedo[4] = { mat->m_colAlbedo.r, mat->m_colAlbedo.g, mat->m_colAlbedo.b, mat->m_colAlbedo.a };
					if(ImGui::ColorEdit4("Albedo", albedo))
						mat->m_colAlbedo = glm::vec4(albedo[0], albedo[1], albedo[2], albedo[3]);

					float emission[4] = { mat->m_colEmission.r, mat->m_colEmission.g, mat->m_colEmission.b, mat->m_colEmission.a };
					if (ImGui::ColorEdit4("Emission", emission))
						mat->m_colEmission = glm::vec4(emission[0], emission[1], emission[2], emission[3]);

					float roughness = mat->m_fRoughness;
					if (ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f))
						mat->m_fRoughness = roughness;

					float metallic = mat->m_fMetallic;
					if (ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f))
						mat->m_fMetallic = metallic;

					float occlusion = mat->m_fOcclusion;
					if (ImGui::SliderFloat("Occlusion", &occlusion, 0.0f, 1.0f))
						mat->m_fOcclusion = occlusion;

					float height = mat->m_fHeight;
					if (ImGui::SliderFloat("Height", &height, 0.0f, 1.0f))
						mat->m_fHeight = height;

					bool showWireframe = mat->m_bWireframe;
					if(ImGui::Checkbox("Wireframe", &showWireframe))
						mat->m_bWireframe = showWireframe;

					ImGui::SameLine();
					bool useDxNormals = mat->m_bDxNormals;
					if (ImGui::Checkbox("Use D3D Normals", &useDxNormals))
						mat->m_bDxNormals = useDxNormals;
				
					ImGui::TreePop();
				}


				if (ImGui::TreeNode("Transform"))
				{
					// Position
					glm::vec3 pos = object->m_vecPosition;
					
					if (ImGui::SliderFloat3("Position", glm::value_ptr(pos), -100, 100))
						object->m_vecPosition = pos;

					// Rotation Axis
					glm::vec3 rot = object->m_vecRotationAxis;
					if (ImGui::InputFloat3("Rotation Axis", glm::value_ptr(rot), 1))
						object->m_vecRotationAxis = rot;

					// Rotation Angle
					float rotAngle = object->m_fAngle;
					if (ImGui::SliderAngle("Rotation Angle", &rotAngle))
						object->m_fCurrentRotationAngle = rotAngle;

					// Scale
					glm::vec3 scale = object->m_vecScale;
					if (ImGui::InputFloat3("Scale", glm::value_ptr(scale)))
						object->m_vecScale = scale;

					// AutoUpdate
					bool autoRotate = object->m_bAutoRotate;
					if (ImGui::Checkbox("Auto Rotate", &autoRotate))
						object->m_bAutoRotate = autoRotate;

					// AutoRotate Speed
					float autoSpeed = object->m_fAutoRotateSpeed;
					if (ImGui::SliderFloat("Rotation Speed", &autoSpeed, 0.01f, 1.0f))
						object->m_fAutoRotateSpeed = autoSpeed;

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
				glm::vec3	angleXYZ = dirLightObject->GetLightAngleEuler();
				if (ImGui::SliderFloat3("Rotation", glm::value_ptr(angleXYZ), -360, 360, "%0.1f"))
					dirLightObject->SetEulerLightAngles(angleXYZ);
		
				glm::vec3	color = dirLightObject->m_vecLightColor;
				if (ImGui::ColorEdit3("Color", glm::value_ptr(color)))
					dirLightObject->m_vecLightColor = color;
		
				float		intensity = dirLightObject->m_fLightIntensity;
				if (ImGui::SliderFloat("Intensity", &intensity, 0, 10))
					dirLightObject->m_fLightIntensity = intensity;

				ImGui::Separator();

				bool bShowShadowBounds = dirLightObject->m_bShowDebugShadowVolume;
				ImGui::Checkbox("Edit Shadow Bounds (Buggy, Crash!)", &bShowShadowBounds);
				dirLightObject->m_bShowDebugShadowVolume = bShowShadowBounds;
				
				if (ImGui::CollapsingHeader("Shadow Bound Properties", &bShowShadowBounds))
				{
					float bound = dirLightObject->m_fShadowBounds;
					if (ImGui::SliderFloat("Bounds", &bound, 1.0f, 100.0f))
					{
						dirLightObject->m_fShadowBounds = bound;
						dirLightObject->UpdateBounds();
					}

					float nearPlane = dirLightObject->m_fShadowNearPlane;
					if (ImGui::SliderFloat("Near Plane", &nearPlane, -50.0f, 100.0f))
					{
						dirLightObject->m_fShadowNearPlane = nearPlane;
						dirLightObject->UpdateBounds();
					}

					float farPlane = dirLightObject->m_fShadowFarPlane;
					if (ImGui::SliderFloat("Far Plane", &farPlane, 1.0f, 200.0f))
					{
						dirLightObject->m_fShadowFarPlane = farPlane;
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
		
				float		intensity = pointLightObject->m_fLightIntensity;
				if (ImGui::SliderFloat("Intensity", &intensity, 0, 10))
					pointLightObject->m_fLightIntensity = intensity;
				
				float		radius = pointLightObject->m_fLightRadius;
				if (ImGui::SliderFloat("Radius", &radius, 0, 50))
					pointLightObject->m_fLightRadius = radius;
		
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
		if(ImGui::SliderFloat("Bloom Cutoff", &bloomCutoff, 0.0f, 50.0f))
		{
			ptrFX->SetBloomThreshold(bloomCutoff);
		}

		float fGamma = ptrFX->GetGamma();
		if (ImGui::SliderFloat("Gamma", &fGamma, 0.0f, 5.0f))
		{
			ptrFX->SetGamma(fGamma);
		}

		float fExposure = ptrFX->GetExposure();
		if (ImGui::SliderFloat("Exposure", &fExposure, 0.0f, 10.0f))
		{
			ptrFX->SetExposure(fExposure);
		}
	}

	static int channelID = 0;
	if (ImGui::CollapsingHeader("Debug Pass"))
	{
		if (ImGui::RadioButton("Final", &channelID, 0))
		{
			ptrFX->SetChannelID(channelID);
		}

		if (ImGui::RadioButton("Direct Lighing", &channelID, 1))
		{
			ptrFX->SetChannelID(channelID);
		}

		if (ImGui::RadioButton("Indirect Diffuse", &channelID, 2))
		{
			ptrFX->SetChannelID(channelID);
		}

		if (ImGui::RadioButton("Shadow Depth", &channelID, 3))
		{
			ptrFX->SetChannelID(channelID);
		}

		if (ImGui::RadioButton("Albedo", &channelID, 4))
		{
			ptrFX->SetChannelID(channelID);
		}

		if (ImGui::RadioButton("Position", &channelID, 5))
		{
			ptrFX->SetChannelID(channelID);
		}

		if (ImGui::RadioButton("Normal", &channelID, 6))
		{
			ptrFX->SetChannelID(channelID);
		}

		if (ImGui::RadioButton("Emission", &channelID, 7))
		{
			ptrFX->SetChannelID(channelID);
		}

		if (ImGui::RadioButton("Roughness", &channelID, 8))
		{
			ptrFX->SetChannelID(channelID);
		}

		if (ImGui::RadioButton("Metallic", &channelID, 9))
		{
			ptrFX->SetChannelID(channelID);
		}

		if (ImGui::RadioButton("Occlusion", &channelID, 10))
		{
			ptrFX->SetChannelID(channelID);
		}

		if (ImGui::RadioButton("Skybox", &channelID, 11))
		{
			ptrFX->SetChannelID(channelID);
		}

		if (ImGui::RadioButton("Object ID", &channelID, 12))
		{
			ptrFX->SetChannelID(channelID);
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




