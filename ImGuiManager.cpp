#include "ImGuiManager.h"
#include "WindowManager.h"

// Widgets
#include "WindowMenuBarWidget.h"
#include "WorldOutlinerWidget.h"
#include "DebugSettingsWidget.h"
// Included here to actually have the implementation for the linker
#include "imgui.cpp"
#include "imgui_draw.cpp"
#include "imgui_demo.cpp"
#include "imgui_impl_glfw_gl3.cpp"

ImGuiManager::~ImGuiManager()
{
	// Cleanup
	ImGui_ImplGlfwGL3_Shutdown();
}

void ImGuiManager::ImGuiNewFrame()
{
	ImGui_ImplGlfwGL3_NewFrame();
}

void ImGuiManager::ImGuiRender()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Makes sure any render modes in the renderer don't affect the ImGui widgets
	ImGui::Render();
}

void ImGuiManager::DrawWidgets()
{
	for (int i = 0; i < WidgetList.size(); ++i)
		WidgetList[i]->DrawWidget();

	// 2. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	//ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
	//ImGui::ShowTestWindow(&show_test_window);

}

void ImGuiManager::OnNotify(Event * aEvent)
{
	EngineEvent * engineEvent = nullptr;
	engineEvent = dynamic_cast<EngineEvent *>(aEvent);

	if (engineEvent)
	{
		if (engineEvent->EventID == EngineEvent::EventList::ENGINE_INIT)
		{
			// Setup ImGui binding
			ImGui_ImplGlfwGL3_Init(EngineHandle.GetWindowManager().GetWindow(), true);

			// Add widgets
			WidgetList.push_back(new WindowMenuBarWidget(*this));
			WidgetList.push_back(new WorldOutlinerWidget(*this));
			WidgetList.push_back(new DebugSettingsWidget(*this));
		}
		else if (engineEvent->EventID == EngineEvent::EventList::ENGINE_TICK)
		{
			DrawWidgets();
		}
		else if (engineEvent->EventID == EngineEvent::EventList::ENGINE_EXIT)
		{
			// Cleanup
			ImGui_ImplGlfwGL3_Shutdown();
		}
		return;
	}

	WindowEvent * windowEvent = nullptr;
	windowEvent = dynamic_cast<WindowEvent *>(aEvent);

	if (windowEvent)
	{
		if (windowEvent->EventID == WindowEvent::WINDOW_RESIZE)
		{
			
		}
		return;
	}
}
