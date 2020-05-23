#pragma once
#include "glm/vec2.hpp"

#include "Engine.h"
#include "Observer.h"
class InputManager : public Observer
{
	/*----------MEMBER VARIABLES----------*/
private:
	bool keyboardStatePrev[GLFW_KEY_LAST];	// Holds the state of all the keyboard keys in the previous frame
	bool keyboardStateCurr[GLFW_KEY_LAST];	// Holds the state of all the keyboard keys in the current frame
public:
	glm::vec2 CurrentMousePosition;
	glm::vec2 PreviousMousePosition;
	glm::vec2 MouseDelta;

	static glm::vec2 CurrentScrollDirection;
	static glm::vec2 PreviousScrollDirection;
	static glm::vec2 ScrollDelta;
	/*------------------------------- ENGINE REFERENCE -------------------------------*/
	Engine & EngineHandle;

	/*----------MEMBER FUNCTIONS----------*/
public:
	InputManager(Engine & aEngine);
	virtual ~InputManager();

	Engine const & GetEngine() { return EngineHandle; }
	Engine const & GetEngine() const { return EngineHandle; }

	// Observer functions
	virtual void OnNotify(Event * aEvent) override;
	
	void Tick();
	/*------------------- KEYBOARD FUNCTIONS ------------------- */
	// Key State Checking
	bool isKeyPressed(int key) const;
	bool isKeyReleased(int key) const;
	bool isKeyTriggered(int key) const;

	void InitializeKeyboardState();

	/*------------------- MOUSE FUNCTIONS ------------------- */
	inline glm::vec2 GetCurrentMousePosition() { return CurrentMousePosition; }
	inline glm::vec2 GetMouseDelta() { return MouseDelta; }

	bool isMouseButtonPressed(int key);

	/*------------------- SCROLLING FUNCTIONS ------------------- */
	inline static glm::vec2 GetCurrentScrollDirection() { return CurrentScrollDirection; }
	inline static glm::vec2 GetScrollDelta() { return ScrollDelta; }

	/*------------------- CURSOR FUNCTIONS ------------------- */
	// This will hide the cursor and lock it to the active window. 
	// Useful for implementing camera controls
	void DisableCursor();
	// Default cursor mode, allows it to move freely
	void EnableCursor();
	// Simply hide the cursor when it is over the window, does not limit its motion
	void HideCursor();
private:
	glm::vec2 GetMousePosition();
	// Scroll
	static void ScrollCallback(GLFWwindow * aWindow, double aXOffset, double aYOffset);
	
};

