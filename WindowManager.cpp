#include <iostream>

#include "WindowManager.h"
#include "ImGuiManager.h"
#include "Engine.h"
// Default initialize width and height
int WindowManager::Width = 1024;
int WindowManager::Height = 768; 
GLFWwindow * WindowManager::pWindow = nullptr;
Subject WindowManager::WindowResize = Subject();

WindowManager::WindowManager(Engine & aEngine) : EngineHandle(aEngine)
{
}

WindowManager::~WindowManager()
{
}

int WindowManager::InitializeWindow()
{
	/* Initialize the library */
	if (!glfwInit())
		return -1;
	
	/* Create a windowed mode window and its OpenGL context */
	pWindow = glfwCreateWindow(Width, Height, "Physics Framework", NULL, NULL);
	if (!pWindow)
	{
		std::cout << "Could not Initialize Window!";
		glfwTerminate();
		return -1;
	}
	/* Make the window's context current */
	glfwMakeContextCurrent(pWindow);

	glfwSetWindowSizeCallback(pWindow, WindowResizeCallback);
	return 0;
}

void WindowManager::WindowResizeCallback(GLFWwindow * aWindow, int aWidth, int aHeight)
{
	Width = aWidth;
	Height = aHeight;
	// Resize window
	glfwSetWindowSize(pWindow, Width, Height);
	glfwGetFramebufferSize(pWindow, &aWidth, &aHeight);
	glViewport(0, 0, aWidth, aHeight);

	// Send window resize event
	WindowEvent windowResizeEvent;
	windowResizeEvent.EventID = WindowEvent::WINDOW_RESIZE;
	WindowResize.NotifyAllObservers(&windowResizeEvent);
}

void WindowManager::OnNotify(Event * aEvent)
{
	// Check if this is an Engine event
	EngineEvent * engineEvent = static_cast<EngineEvent *>(aEvent);
	if (engineEvent)
	{
		if(engineEvent->EventID == EngineEvent::EventList::ENGINE_INIT)
		{
			InitializeWindow();
			// Add observers to EngineResize event
			WindowResize.AddObserver(&EngineHandle.GetImGuiManager());
		}
		else if (engineEvent->EventID == EngineEvent::EventList::ENGINE_EXIT)
		{
			glfwTerminate();
		}
		return;
	}
}

