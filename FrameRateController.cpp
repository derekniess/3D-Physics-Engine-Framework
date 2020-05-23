#include "FrameRateController.h"

void FramerateController::InitializeFrameRateController()
{
	TotalTime = NewTime = CurrentTime = DeltaTime = 0.0;
	FixedDelta = 0.016f; 
}

void FramerateController::SetFrameRateLimit(unsigned int Limit)
{
	DeltaTime = 1.0f / Limit;
	// Starts the clock
	glfwSetTime(0);
	CurrentTime = (float)glfwGetTime();
}
void FramerateController::UpdateFrameTime()
{
	NewTime = (float)glfwGetTime();
	DeltaTime = NewTime - CurrentTime;
	CurrentTime = NewTime;
	TotalTime += DeltaTime;
}

FramerateController::~FramerateController()
{
}

void FramerateController::OnNotify(Event * aEvent)
{
	// Check if this is an Engine event
	EngineEvent * engineEvent = static_cast<EngineEvent *>(aEvent);
	if (engineEvent)
	{
		switch (engineEvent->EventID)
		{
			case EngineEvent::EventList::ENGINE_INIT:
			{
				InitializeFrameRateController();
				break;
			}
			case EngineEvent::EventList::ENGINE_TICK:
			{
				UpdateFrameTime();
				break;
			}
		}
	}
}
