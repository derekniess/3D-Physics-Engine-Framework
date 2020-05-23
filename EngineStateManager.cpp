#include "EngineStateManager.h"
#include "InputManager.h"

void EngineStateManager::OnNotify(Event * aEvent)
{
	// Check if this is an Engine event
	EngineEvent * engineEvent = nullptr;
	engineEvent = dynamic_cast<EngineEvent *>(aEvent);

	if (engineEvent)
	{
		if (engineEvent->EventID == EngineEvent::EventList::ENGINE_INIT)
		{
			bShouldRenderCollidersAndNormals = false;
			bRenderModeWireframe = false;
		}
		else if (engineEvent->EventID == EngineEvent::EventList::ENGINE_TICK)
		{
			Update();
		}
		return;
	}
}

void EngineStateManager::Update()
{
	// Wireframe draw check
	if (EngineHandle.GetInputManager().isKeyPressed(GLFW_KEY_F1))
		bRenderModeWireframe = true;
	else if(EngineHandle.GetInputManager().isKeyReleased(GLFW_KEY_F1)) // Allows for widgets to set engine state variables without interference
		bRenderModeWireframe = false;

	// Colliders and normals draw check
	if (EngineHandle.GetInputManager().isKeyPressed(GLFW_KEY_F2))
		bShouldRenderCollidersAndNormals = true;
	else if (EngineHandle.GetInputManager().isKeyReleased(GLFW_KEY_F2))
		bShouldRenderCollidersAndNormals = false;
	

}
