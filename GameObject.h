#pragma once
#include <vector>
#include <memory>

#include "Object.h"
#include "Component.h"
#include "Event.h"
class Engine;

class GameObject : public Observer
{
	/*----------MEMBER VARIABLES----------*/
public:
	std::vector<std::unique_ptr<Component>> ComponentList;	
	std::string Name;

	Engine & EngineHandle;
	/*----------MEMBER FUNCTIONS----------*/
public:
	GameObject(Engine & aEngineHandle) : EngineHandle(aEngineHandle) {}
	virtual ~GameObject() {}
	
	template <typename T> T * GetComponent()
	{
		return dynamic_cast<T *>(GetComponent(T::GetComponentID()));
	}

	std::vector<std::unique_ptr<Component>> const & GetComponentList() { return ComponentList; }
	
	void AddComponent(Component * aNewComponent);

private:
	virtual void OnNotify(Event * aEvent) override;

	Component * GetComponent(Component::ComponentType);
	
	// Calls Update() of all it's components
	void Update();
	// Calls Initialize() of all it's components
	void Initialize();
	// Calls Destroy() of all it's component
	void Destroy();
};
