#pragma once

#include <map>
#include <memory>
// GLEW
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
// Entity classes
#include "Object.h"
// Event classes
#include "Event.h"
#include "Subject.h"

// Forward Declarations
class WindowManager;
class InputManager;
class FramerateController;
class ResourceManager;
class Renderer;
class PhysicsManager;
class ImGuiManager;
class EngineStateManager;
class DebugFactory;
class GameObjectFactory;

// The main engine events that have Subjects created from the start
class EngineEvent : public Event
{
public:
	enum EventList
	{
		ENGINE_INIT,
		ENGINE_LOAD,
		ENGINE_TICK,
		ENGINE_UNLOAD,
		ENGINE_EXIT
	};
	EventList EventID;
	EngineEvent() {};
	~EngineEvent() {};
};

class Engine : public Object
{
	/*----------MEMBER VARIABLES----------*/
	private:
	std::unique_ptr<WindowManager> pWindowManager;
	std::unique_ptr<InputManager> pInputManager;
	std::unique_ptr<FramerateController> pFrameRateController;
	std::unique_ptr<ResourceManager> pResourceManager;
	std::unique_ptr<Renderer> pRenderer;
	std::unique_ptr<PhysicsManager> pPhysicsManager;
	std::unique_ptr<ImGuiManager> pImGuiManager;
	std::unique_ptr<EngineStateManager> pEngineStateManager;
	std::unique_ptr<GameObjectFactory> pGameObjectFactory;
	std::unique_ptr<DebugFactory> pDebugFactory;

	// A map of the main engine events
	std::map<EngineEvent::EventList, Subject> MainEventList;

	/*----------MEMBER FUNCTIONS----------*/
public:
	Engine();
	// Initialize all managers, factories and high level systems
	void Init();
	// Initialize all gameobjects, components, widgets and low level systems
	void Load();
	int Unload();
	void Exit();
	void Tick();
	virtual ~Engine();
	std::map<EngineEvent::EventList, Subject> & GetMainEventList() { return MainEventList; }

	inline WindowManager & GetWindowManager() { return *pWindowManager; }
	inline InputManager & GetInputManager() { return *pInputManager; }
	inline FramerateController & GetFramerateController() { return *pFrameRateController; }
	inline ResourceManager & GetResourceManager() { return *pResourceManager; }
	inline Renderer & GetRenderer() { return *pRenderer; }
	inline PhysicsManager & GetPhysicsManager() { return *pPhysicsManager; }
	inline ImGuiManager & GetImGuiManager() { return *pImGuiManager; }
	inline EngineStateManager & GetEngineStateManager() { return *pEngineStateManager; }
	inline GameObjectFactory & GetGameObjectFactory() { return *pGameObjectFactory; }
	inline DebugFactory & GetDebugFactory() { return *pDebugFactory; }

};

