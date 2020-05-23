#include <glm/gtc/random.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "Engine.h"
#include "WindowManager.h"
#include "InputManager.h"
#include "GameObjectFactory.h"
#include "EngineStateManager.h"
#include "FrameRateController.h"
#include "PhysicsManager.h"
#include "Sprite.h"
#include "Camera.h"
#include "Controller.h"
#include "ImGuiManager.h"
#include "DebugFactory.h"
#include "Grid.h"
#include "Script.h"
#include "WaveSolver.h"
#include "Box.h"

Engine::Engine()
{
	/*-------------- MANAGER CREATION --------------*/
	pFrameRateController = std::make_unique<FramerateController>(*this);
	pEngineStateManager = std::make_unique<EngineStateManager>(*this);
	pResourceManager = std::make_unique<ResourceManager>(*this);
	pWindowManager = std::make_unique<WindowManager>(*this);
	pInputManager = std::make_unique<InputManager>(*this);
	pPhysicsManager = std::make_unique<PhysicsManager>(*this);
	pRenderer = std::make_unique<Renderer>(*this);
	pImGuiManager = std::make_unique<ImGuiManager>(*this);

	/*-------------- FACTORY CREATION --------------*/
	pGameObjectFactory = std::make_unique<GameObjectFactory>(*this);
	pDebugFactory = std::make_unique<DebugFactory>(*this);

	/*-------------- ENGINE INIT EVENT REGISTRATION --------------*/
	Subject EngineInitialized;
	
	// Register Managers as observers to engine initialization event
	EngineInitialized.AddObserver(pFrameRateController.get());
	EngineInitialized.AddObserver(pEngineStateManager.get());
	EngineInitialized.AddObserver(pWindowManager.get());
	EngineInitialized.AddObserver(pInputManager.get());
	EngineInitialized.AddObserver(pRenderer.get());
	EngineInitialized.AddObserver(pImGuiManager.get());
	EngineInitialized.AddObserver(pGameObjectFactory.get());
	EngineInitialized.AddObserver(pDebugFactory.get());

	// Adds the engine initialized subject to the main event list (must be done after adding all observers as emplace uses a copy in a map)
	MainEventList.emplace(std::make_pair(EngineEvent::ENGINE_INIT, EngineInitialized));
	
	/*-------------- ENGINE LOAD EVENT REGISTRATION --------------*/
	Subject EngineLoad;
	
	EngineLoad.AddObserver(pRenderer.get());
	EngineLoad.AddObserver(pResourceManager.get());

	MainEventList.emplace(std::make_pair(EngineEvent::ENGINE_LOAD, EngineLoad));

	/*-------------- ENGINE TICK EVENT REGISTRATION --------------*/
	Subject EngineTick;

	EngineTick.AddObserver(pFrameRateController.get());
	EngineTick.AddObserver(pEngineStateManager.get());
	EngineTick.AddObserver(pInputManager.get());
	EngineTick.AddObserver(pRenderer.get());
	EngineTick.AddObserver(pPhysicsManager.get());

	EngineTick.AddObserver(pImGuiManager.get());
	MainEventList.emplace(std::make_pair(EngineEvent::ENGINE_TICK, EngineTick));
	
	/*-------------- ENGINE EXIT EVENT REGISTRATION --------------*/
	Subject EngineExit;

	EngineExit.AddObserver(pWindowManager.get());
	EngineExit.AddObserver(pImGuiManager.get());

	MainEventList.emplace(std::make_pair(EngineEvent::ENGINE_EXIT, EngineExit));

}

void Engine::Init()
{
	EngineEvent InitEvent;
	InitEvent.EventID = EngineEvent::ENGINE_INIT;
	// Notify all listeners to engine init (called to initialize managers and factories)
	MainEventList[EngineEvent::ENGINE_INIT].NotifyAllObservers(&InitEvent);

	// Create camera and add it to the tick notification list
	Camera * mainCamera = new Camera(*pInputManager, *pFrameRateController);
	mainCamera->SetCameraPosition(glm::vec3(0, 5, -15));
	mainCamera->SetCameraLookDirection(glm::vec3(0, 0, 1));
	MainEventList[EngineEvent::ENGINE_TICK].AddObserver(mainCamera);
	// Set the renderer camera reference
	pRenderer->SetActiveCamera(mainCamera);

	return;
}

void Engine::Load()
{
	EngineEvent LoadEvent;
	LoadEvent.EventID = EngineEvent::ENGINE_LOAD;

	/*-----------------EDITOR OBJECTS INITIALIZTION--------------------*/

	// TODO : [@Derek] -  Consider adding a separate type of editor object 
	// Initialize pivot 
	GameObject * pivot = pGameObjectFactory->SpawnGameObject();
	Mesh * pivotMesh = pResourceManager->ImportMesh(std::string("Pivot.fbx"));
	pivot->GetComponent<Transform>()->SetScale(glm::vec3(0.1f, -0.1f, -0.1f));
	pivot->AddComponent(pivotMesh);
	pivotMesh->RenderDebug = false;

	//GameObject * grid = pGameObjectFactory->SpawnGameObject();
	//Primitive * gridPrimitive = pGameObjectFactory->SpawnComponent<Primitive>();
	//gridPrimitive->RenderDebug = false;
	//gridPrimitive->bIsDebug = false;
	//Grid newGrid(50, 50, 10, 10);
	//pRenderer->RegisterPrimitive(gridPrimitive);
	//// SET VERTICES AFTER REGISTRATION, a primitive must be registered if it is to bind
	//gridPrimitive->SetVertices(newGrid.GridVertices);
	//grid->AddComponent(gridPrimitive);
	/*-----------------GAME OBJECTS INITIALIZTION--------------------*/

	// Falling cube
	GameObject * cube1 = pGameObjectFactory->SpawnGameObject();
	cube1->GetComponent<Transform>()->SetPosition(glm::vec3(0.0f, 5.0f, 3.0f));
	Mesh * cube1Mesh = pResourceManager->ImportMesh(std::string("Cube.fbx"));
	cube1->AddComponent(cube1Mesh);
	Physics * physics1 = pGameObjectFactory->SpawnComponent<Physics>();
	cube1->AddComponent(physics1);
	Collider * cube1Collider = pGameObjectFactory->SpawnComponent<Box>();
	cube1->AddComponent(cube1Collider);

	// Ground
	GameObject * cube2 = pGameObjectFactory->SpawnGameObject();
	cube2->GetComponent<Transform>()->SetPosition(glm::vec3(0.0f, -4.0f, 3.0f));
	cube2->GetComponent<Transform>()->SetScale(glm::vec3(10.f, 1.f, 10.0f));
	Mesh * cube2Mesh = pResourceManager->ImportMesh(std::string("Cube.fbx"));
	cube2->AddComponent(cube2Mesh);
	Physics * physics2 = pGameObjectFactory->SpawnComponent<Physics>();
	cube2->AddComponent(physics2);
	Collider * cube2Collider = pGameObjectFactory->SpawnComponent<Box>();
	cube2->AddComponent(cube2Collider);
	cube2Collider->eColliderType = Collider::STATIC;

	// Notify all listeners to engine load
	MainEventList[EngineEvent::ENGINE_LOAD].NotifyAllObservers(&LoadEvent);

	return;
}

void Engine::Exit()
{
	EngineEvent ExitEvent;
	ExitEvent.EventID = EngineEvent::ENGINE_EXIT;
	// Notify all listeners to engine exit
	MainEventList[EngineEvent::ENGINE_EXIT].NotifyAllObservers(&ExitEvent);

	return;
}

void Engine::Tick()
{

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(pWindowManager->GetWindow()))
	{
		// Needs to be called at start of every frame
		ImGuiManager::ImGuiNewFrame();

		glClear(GL_COLOR_BUFFER_BIT);

		EngineEvent TickEvent;
		TickEvent.EventID = EngineEvent::ENGINE_TICK;
		// Notify all listeners to engine tick 
		MainEventList[EngineEvent::ENGINE_TICK].NotifyAllObservers(&TickEvent);

		// Draws GUI widgets on top of everything else
		ImGuiManager::ImGuiRender();
		/* Swap front and back buffers */
		glfwSwapBuffers(pWindowManager->GetWindow());


		if (pInputManager->isKeyReleased(GLFW_KEY_ESCAPE))
			break;

		/* Poll for and process events */
		glfwPollEvents();

	}

	Exit();
	return;
}


Engine::~Engine()
{
}
