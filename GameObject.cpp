#include "GameObject.h"
#include "Engine.h"
#include "Physics.h"
#include "Transform.h"
#include "Controller.h"

void GameObject::OnNotify(Event * aEvent)
{
	// Check if this is an Engine event
	EngineEvent * engineEvent = static_cast<EngineEvent *>(aEvent);
	if (engineEvent)
	{
		if (engineEvent->EventID == EngineEvent::EventList::ENGINE_LOAD)
		{
			Initialize();
		}
		else if(engineEvent->EventID == EngineEvent::EventList::ENGINE_TICK)
		{
			Update();
		}
		return;
	}
}

void GameObject::Initialize()
{
	for (int i = 0; i < ComponentList.size(); ++i)
	{
		ComponentList[i]->Initialize();
	}
}

void GameObject::Destroy()
{
	for (int i = 0; i < ComponentList.size(); ++i)
	{
		ComponentList[i]->Destroy();
	}
}

void GameObject::Update()
{
	for (int i = 0; i < ComponentList.size(); ++i)
	{
		ComponentList[i]->Update();
	}
}

Component * GameObject::GetComponent(Component::ComponentType aType)
{
	for (int i = 0; i < ComponentList.size(); ++i)
	{
		if (ComponentList[i]->GetComponentType() == aType)
			return (ComponentList[i].get());
	}
	
	//std::cout << "Component of that type does not exist in GameObject:" << std::endl;
	return nullptr;
}

// Taken from https://mbevin.wordpress.com/2012/11/18/smart-pointers/
// "emplace_back works like push_back, 
// but allows you to simply pass the arguments you would otherwise pass to the constructor 
// for the object which you're storing in the vector, rather than passing the object itself. 
// This method can also be more efficient than using the old push_back, so should be generally preferred."
void GameObject::AddComponent(Component * aNewComponent)
{
	ComponentList.emplace_back(aNewComponent);
	aNewComponent->SetOwner(this);
	
	Physics * aNewPhysics = nullptr;
	aNewPhysics = dynamic_cast<Physics *>(aNewComponent);
	if (aNewPhysics)
	{
		Transform * transform = aNewPhysics->GetOwner()->GetComponent<Transform>();
		aNewPhysics->SetCurrentPosition(transform->GetPosition());
		aNewPhysics->SetNextPosition(transform->GetPosition());
		return;
	}

	Controller * aNewController = nullptr;
	aNewController = dynamic_cast<Controller *>(aNewComponent);
	if (aNewController)
	{
		Transform * ownerTransform = aNewController->GetOwner()->GetComponent<Transform>();
		aNewController->TargetTransform = ownerTransform;
	}
}
