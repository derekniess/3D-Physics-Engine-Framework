#pragma once
#include "ImGuiWidget.h"
#include "GameObject.h"

class Transform;
class InputManager;

class WorldOutlinerWidget : public ImGuiWidget
{
public:
	void DrawGameObjectProperties(const char* gameObjectName, GameObject * pGameObject, InputManager & aInputManager);
	void DrawTransformProperties(Transform * aTransform, InputManager & aInputManager);

	virtual bool DrawWidget() override;

	WorldOutlinerWidget(ImGuiManager & aManager) :
		ImGuiWidget(aManager) {}
	// Transform panel values
	float xAngle = 0.0f;
	float yAngle = 0.0f;
	float zAngle = 0.0f;

	float CurrentRotation[3] = { 0, 0, 0 };
	float PreviousRotation[3] = { 0, 0, 0 };
};