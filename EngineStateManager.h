#pragma once
#include "Observer.h"

class Engine;

class EngineStateManager : public Observer
{
	/*----------MEMBER VARIABLES----------*/
public:
	// Engine Reference
	Engine & EngineHandle;
	// Renderer variables
	bool bShouldRenderCollidersAndNormals = false;
	bool bRenderModeWireframe = false;
	bool bShouldRenderMinkowskiDifference = false;
	// Physics variables
	int NumberofIterations;
	bool bShouldSimulationRun = true;
	bool bContactDebugModeEnabled = false;
	bool bShouldRenderSimplex = false;
	bool bUseEulerIntegration = true;
	bool bUseVerletIntegration = false;
	bool bUseRK4Integration = false;
	/*----------MEMBER FUNCTIONS----------*/
public:
	EngineStateManager(Engine & aEngine) : EngineHandle(aEngine) {}
	~EngineStateManager() {}

	Engine const & GetEngine() { return EngineHandle; }
private:
	void Update();

	// Observer functions
	virtual void OnNotify(Event * aEvent) override;

};
