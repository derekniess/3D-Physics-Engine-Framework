#include <glm/gtc/type_ptr.hpp>

#include "WorldOutlinerWidget.h"
#include "WindowManager.h"
#include "GameObjectFactory.h"
#include "InputManager.h"
#include "Engine.h"

bool WorldOutlinerWidget::DrawWidget()
{
	int height = ImGuiManagerReference.EngineHandle.GetWindowManager().Height;
	int width = ImGuiManagerReference.EngineHandle.GetWindowManager().Width;
	InputManager & inputManager = ImGuiManagerReference.EngineHandle.GetInputManager();
	
	if (!ImGui::Begin("World Outliner"))
	{
		ImGui::End();
		return false;
	}


	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
	ImGui::Columns(2);
	ImGui::Separator();

	ImGui::Text("Object Name");
	ImGui::NextColumn();
	ImGui::Text("Object Properties");
	ImGui::NextColumn();

	// Iterate through all game objects and display their properties
	std::vector<std::unique_ptr<GameObject>> & gameObjectList = ImGuiManagerReference.EngineHandle.GetGameObjectFactory().GameObjectList;
	for (int i = 0; i < gameObjectList.size(); ++i)
	{
		std::string gameObjectName = gameObjectList[i]->Name;
		DrawGameObjectProperties(gameObjectName.c_str(), gameObjectList[i].get(), inputManager);
		ImGui::Spacing();
		ImGui::Separator();
	}
	ImGui::Columns(1);
	ImGui::Separator();
	ImGui::PopStyleVar();
	ImGui::End();
	return true;
}

void WorldOutlinerWidget::DrawGameObjectProperties(const char* gameObjectName, GameObject * pGameObject, InputManager & aInputManager)
{
	ImGui::PushID(pGameObject);             // Use object uid as identifier. Most commonly you could also use the object pointer as a base ID.
	ImGui::AlignFirstTextHeightToWidgets();  // Text and Tree nodes are less high than regular widgets, here we add vertical spacing to make the tree lines equal high.
	bool node_open = ImGui::TreeNode("%s_%u", gameObjectName, pGameObject);
	if (node_open)
	{
		std::vector<std::unique_ptr<Component>> & componentList = pGameObject->ComponentList;
		ImGui::NextColumn();
		ImGui::Spacing();
		ImGui::NextColumn();

		for (int i = 0; i < componentList.size(); ++i)
		{
			ImGui::NewLine();
			// If a transform component
			Transform * transform = nullptr;
			transform = dynamic_cast<Transform *>(componentList[i].get());
			if (transform)
			{
				DrawTransformProperties(transform, aInputManager);
			}
		}

		ImGui::TreePop();
	}
	ImGui::PopID();
}

void WorldOutlinerWidget::DrawTransformProperties(Transform * aTransform, InputManager & aInputManager)
{
	ImGui::PushID(aTransform); // Use pointer as identifier.

	char componentNameLabel[32];
	sprintf(componentNameLabel, "%s", aTransform->GetComponentName());
	ImGui::Bullet();
	ImGui::Selectable(componentNameLabel);
	ImGui::NextColumn();
	// Position
	ImGui::InputFloat3(":Position", glm::value_ptr(aTransform->Position));

	// Rotation
	// X Rotation Slider
	ImGui::PushItemWidth(50);
	if (ImGui::SliderAngle(":X", &xAngle, 0, 360))
	{
		PreviousRotation[0] = CurrentRotation[0];
		CurrentRotation[0] = xAngle;

		if (aInputManager.CurrentMousePosition != aInputManager.PreviousMousePosition)
		{
			xAngle = CurrentRotation[0] - PreviousRotation[0];
			glm::quat xRotation = glm::angleAxis(xAngle, glm::vec3(1, 0, 0));
			aTransform->Rotate(xRotation);
		}
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();

	// Y Rotation Slider
	ImGui::PushItemWidth(50);
	if (ImGui::SliderAngle(":Y", &yAngle, 0, 360))
	{
		PreviousRotation[1] = CurrentRotation[1];
		CurrentRotation[1] = yAngle;

		if (aInputManager.CurrentMousePosition != aInputManager.PreviousMousePosition)
		{
			yAngle = CurrentRotation[1] - PreviousRotation[1];
			glm::quat yRotation = glm::angleAxis(yAngle, glm::vec3(0, 1, 0));
			aTransform->Rotate(yRotation);
		}
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();

	// Z Rotation Slider
	ImGui::PushItemWidth(50);
	if (ImGui::SliderAngle(":Z", &zAngle, 0, 360))
	{
		PreviousRotation[2] = CurrentRotation[2];
		CurrentRotation[2] = zAngle;

		if (aInputManager.CurrentMousePosition != aInputManager.PreviousMousePosition)
		{
			float zAngle = CurrentRotation[2] - PreviousRotation[2];
			glm::quat zRotation = glm::angleAxis(zAngle, glm::vec3(0, 0, 1));
			aTransform->Rotate(zRotation);
		}
	}
	ImGui::PopItemWidth();

	// Scale 
	ImGui::InputFloat3(":Scale", glm::value_ptr(aTransform->Scale));
	ImGui::NextColumn();

	ImGui::PopID();
}
