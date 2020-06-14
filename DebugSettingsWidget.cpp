#include "DebugSettingsWidget.h"
#include "EngineStateManager.h"
#include "PhysicsManager.h"
#include "Renderer.h"
#include "Engine.h"

bool DebugSettingsWidget::DrawWidget()
{
	bool show_test_window = true;
	// 1. Show a simple window
	// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
	if(ImGui::Begin("Debug Settings"))
	{
		ImGui::Text("Use WASD & QE to move the camera.");

		EngineStateManager & engineStateManager = ImGuiManagerReference.EngineHandle.GetEngineStateManager();
		PhysicsManager & physicsManager = ImGuiManagerReference.EngineHandle.GetPhysicsManager();

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	
		ImGui::Checkbox("Wireframe Mode (F1)", &engineStateManager.bRenderModeWireframe);

		ImGui::PushItemWidth(150);
		ImGui::SliderInt("Wireframe Thickness: ", &Renderer::WireframeThickness, 1, 5);
		ImGui::PopItemWidth();

		ImGui::PushItemWidth(150);
		ImGui::SliderInt("Line Loop Thickness: ", &Renderer::LineLoopThickness, 1, 6);
		ImGui::PopItemWidth();

		ImGui::Checkbox("Render Colliders and Normals (F2)", &engineStateManager.bShouldRenderCollidersAndNormals);

		ImGui::Checkbox("Render Simplex ", &engineStateManager.bShouldRenderSimplex);

		ImGui::Checkbox("Render Minkowski Difference ", &engineStateManager.bShouldRenderMinkowskiDifference);
	
		ImGui::Checkbox("Simulation Enabled ", &engineStateManager.bShouldSimulationRun);

		ImGui::Checkbox("Contact Debug Mode Enabled ", &engineStateManager.bContactDebugModeEnabled);

		ImGui::PushItemWidth(150);
		ImGui::SliderInt("Integrator Iterations: ", &PhysicsManager::IntegratorIterations, 1, 100);
		ImGui::PopItemWidth();

		ImGui::End();
		return true;
	}
	
	return false;
}
