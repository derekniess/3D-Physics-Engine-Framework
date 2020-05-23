#pragma once
// Components
#include "Primitive.h"
//Entities
#include "GameObject.h"
// Render Utilities
#include "Arrow.h"
#include "LineLoop.h"
#include "Quad.h"

class DebugFactory : public Observer
{
	/*----------MEMBER VARIABLES----------*/
public:
	Engine & EngineHandle;

	GameObject * MinkowskiDifference;
	std::vector<Arrow> DebugArrowsStack;
	std::vector<Quad> DebugQuadsStack;
	std::vector<LineLoop> DebugLineLoopsStack;

	/*----------MEMBER FUNCTIONS----------*/
public:
	DebugFactory(Engine & aEngine) : EngineHandle(aEngine)
	{}

	// Pushes debug line onto a stack of arrows to be drawn
	void RegisterDebugArrow(Arrow & aLine);
	// Pushes debug line loop onto a stack of line loops to be drawn
	void RegisterDebugLineLoop(LineLoop &aLineLoop);
	// Pushes debug quad onto a stack of quads to be drawn
	void RegisterDebugQuad(Quad & aQuad);

	virtual void OnNotify(Event * aEvent) override;

	virtual ~DebugFactory() {};
};