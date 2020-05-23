#pragma once
#include "ImGuiWidget.h"
#include "GameObject.h"

class Transform;

class DebugSettingsWidget : public ImGuiWidget
{
public:
	virtual bool DrawWidget() override;

	DebugSettingsWidget(ImGuiManager & aManager) :
		ImGuiWidget(aManager) {}
};