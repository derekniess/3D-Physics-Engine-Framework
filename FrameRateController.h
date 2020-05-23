#pragma once
#include "Engine.h"
class FramerateController : public Observer
{
	/*----------MEMBER FUNCTIONS----------*/
public:
	FramerateController(Engine const & aEngine) : EngineHandle(aEngine) {}
	~FramerateController();

	virtual void OnNotify(Event * aEvent);
	Engine const & GetEngine() { return EngineHandle; }

private:
	void InitializeFrameRateController();
	void SetFrameRateLimit(unsigned int Limit);
	void UpdateFrameTime();
	/*----------MEMBER VARIABLES----------*/
public:
	// Total time accumulator
	float TotalTime;
	// Frame time limit
	float FixedDelta;
	// Actual time since last frame
	float DeltaTime;
	// Current time in seconds
	float CurrentTime;
	// New time in seconds
	float NewTime;
private:
	/*------------------------------- ENGINE REFERENCE -------------------------------*/
	Engine const & EngineHandle;

};

