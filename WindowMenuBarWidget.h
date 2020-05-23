#pragma once
#include "ImGuiWidget.h"

class WindowMenuBarWidget : public ImGuiWidget
{
private:
	void DrawFileMenu();
public:
	virtual bool DrawWidget() override;
	WindowMenuBarWidget(ImGuiManager & aManager) : 
		ImGuiWidget(aManager) {}
};