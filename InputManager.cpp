#include "InputManager.h"
#include "WindowManager.h"
#include "glfw\glfw3.h"
// Static member initialization
glm::vec2 InputManager::CurrentScrollDirection = glm::vec2(0);
glm::vec2 InputManager::PreviousScrollDirection;
glm::vec2 InputManager::ScrollDelta;

// Function called whenever a scrolling device (mouse wheel, touchpad) is used
void InputManager::ScrollCallback(GLFWwindow * aWindow, double aXOffset, double aYOffset)
{
	// Store previous scroll direction
	PreviousScrollDirection = CurrentScrollDirection;
	// Get current scroll direction
	CurrentScrollDirection = glm::vec2(aXOffset, aYOffset);
	// Compare them and find scroll delta
	ScrollDelta = CurrentScrollDirection - PreviousScrollDirection;
}


InputManager::InputManager(Engine & aEngine) : EngineHandle(aEngine)
{
	// Upon construction set callback function for scroll input
	glfwSetScrollCallback(EngineHandle.GetWindowManager().GetWindow(), InputManager::ScrollCallback);
}

void InputManager::InitializeKeyboardState()
{
	for (int i = 0; i < GLFW_KEY_LAST; ++i) // Initializes the value of all keyboard keys to false currently and previously
	{
		keyboardStatePrev[i] = false;
		keyboardStateCurr[i] = false;
	}
}

// This will hide the cursor and lock it to the active window. 
// Useful for implementing camera controls
void InputManager::DisableCursor()
{
	glfwSetInputMode(EngineHandle.GetWindowManager().GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

// Default cursor mode, allows it to move freely
void InputManager::EnableCursor()
{
	glfwSetInputMode(EngineHandle.GetWindowManager().GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void InputManager::HideCursor()
{
	glfwSetInputMode(EngineHandle.GetWindowManager().GetWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}


glm::vec2 InputManager::GetMousePosition()
{
	double screenXPosition, screenYPosition;
	glfwGetCursorPos(EngineHandle.GetWindowManager().GetWindow(), &screenXPosition, &screenYPosition);
	return glm::vec2(screenXPosition, screenYPosition);
}


InputManager::~InputManager()
{
}

void InputManager::OnNotify(Event * aEvent)
{
	// Check if this is an Engine event
	EngineEvent * engineEvent = static_cast<EngineEvent *>(aEvent);
	if (engineEvent)
	{
		switch (engineEvent->EventID)
		{
			case EngineEvent::EventList::ENGINE_INIT:
			{
				InitializeKeyboardState();
				break;
			}
			case EngineEvent::EventList::ENGINE_TICK:
			{
				Tick();
				break;
			}
		}
	}
}

void InputManager::Tick()
{	
	// Check the input state of each 'named' keyboard key
	for (int key = 0; key < GLFW_KEY_LAST; ++key)
	{
		int state = glfwGetKey(EngineHandle.GetWindowManager().GetWindow(), key);

		// If pressed and currently false, set to currently true
		if (state == GLFW_PRESS && keyboardStateCurr[key] == false)
		{
			keyboardStateCurr[key] = true;
		}
		// If pressed and currently true, set to previously true
		else if (state == GLFW_PRESS && keyboardStateCurr[key] == true)
		{
			keyboardStatePrev[key] = true;
		}
		// if not pressed and currently true, set to currently false
		else if (state == GLFW_RELEASE && keyboardStateCurr[key] == true)
		{
			keyboardStateCurr[key] = false;
			keyboardStatePrev[key] = true;
		}
		// if not pressed and previously true, set to previously false
		else if (state == GLFW_RELEASE && keyboardStatePrev[key] == true)
		{
			keyboardStateCurr[key] = false;
			keyboardStatePrev[key] = false;
		}
	}

	// Store previous mouse position
	PreviousMousePosition = CurrentMousePosition;
	// Get mouse position this frame
	CurrentMousePosition = GetMousePosition();
	// Compare the two to get the delta for this frame
	MouseDelta = CurrentMousePosition - PreviousMousePosition;

}

bool InputManager::isKeyPressed(int key) const
{
	return keyboardStateCurr[key] && keyboardStatePrev[key];	// When a key is pressed in both this frame and previous frame returns true
}

bool InputManager::isKeyReleased(int key) const
{
	return !keyboardStateCurr[key] && keyboardStatePrev[key]; 	// When key is not pressed in this frame but pressed in previous frame return true
}

bool InputManager::isKeyTriggered(int key) const
{
	return keyboardStateCurr[key] && !keyboardStatePrev[key];	// True in current frame but not in previous frame
}

