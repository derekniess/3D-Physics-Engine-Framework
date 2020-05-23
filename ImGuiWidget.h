#pragma once
#include "ImGuiManager.h"

class ImGuiWidget
{
public:
	ImGuiManager & ImGuiManagerReference;
public:
	virtual bool DrawWidget() = 0;
	ImGuiWidget(ImGuiManager & aManager) : 
		ImGuiManagerReference(aManager) {}
	
};