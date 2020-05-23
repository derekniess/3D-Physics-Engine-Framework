#pragma once
#include "Component.h"
class Script;

struct ScriptBehavior 
{
	/*-----------MEMBER VARIABLES-----------*/
public:
	// A list of what component types are necessary in order to run this script behavior
	std::vector<Component::ComponentType> ComponentPrerequisites;
	Script * pOwningScript;
	~ScriptBehavior() {}
	/*-----------MEMBER FUNCTIONS-----------*/
public:
	virtual void Initialize() {}
	virtual void Destroy() {}
	virtual void Behavior() = 0 { std::cout << "ScriptBehavior has not implemented a Behavior function!\n"; }
	
};