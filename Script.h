#pragma once
#include "Component.h"
#include "GameObject.h"
#include "ScriptBehavior.h"

class Script : public Component
{
	/*-----------MEMBER VARIABLES-----------*/
public:
	// Every script has a pointer to a 'ScriptBehavior' 
	// which defines a 'Behavior' function that takes as an argument the calling Script and returns nothing
	ScriptBehavior * pScriptBehavior;

	/*-----------MEMBER FUNCTIONS-----------*/
public:
	Script() : Component(Component::SCRIPT) {}
	~Script() { delete pScriptBehavior; }

	static inline ComponentType GetComponentID() { return Component::ComponentType::SCRIPT; }

	virtual void Deserialize(TextFileData aTextData) override {};
	
	// Binds a chosen ScriptBehavior to this Script
	inline void SetBehavior(ScriptBehavior * aScriptBehavior) {
		pScriptBehavior = aScriptBehavior;
		aScriptBehavior->pOwningScript = this;
	}

	inline void CallScript() {
		if (pScriptBehavior != nullptr)
			pScriptBehavior->Behavior();
		else
		{
			std::cout << "CallScript() : Script Belonging to: " << pOwner->Name << " does not have any behavior bound to it!";
		}
	}
	// Script update calls the bound ScriptBehavior
	inline virtual void Update() override { 
		CallScript(); 
	};
	// Call Initialize() of bound ScriptBehavior
	virtual void Initialize() override {
		if (pScriptBehavior != nullptr)
			pScriptBehavior->Initialize();
		else
		{
			std::cout << "CallScript() : Script Belonging to: " << pOwner->Name << " does not have any behavior bound to it!";
		}
	};

	// Call Destroy() of bound ScriptBehavior
	virtual void Destroy() override {
		if (pScriptBehavior != nullptr) {
			pScriptBehavior->Destroy();
		}
	}
};
