#pragma once
#include <sstream>
#include <vector>
#include <typeinfo>
#include <typeindex>

// MANAGERS
#include "Renderer.h"
#include "InputManager.h"
#include "FrameRateController.h"
#include "PhysicsManager.h"

#include "Observer.h"
// GAME OBJECT
#include "GameObject.h"
// COMPONENTS
#include "Transform.h"
#include "Physics.h"
#include "Sprite.h"
#include "Mesh.h"
#include "Controller.h"
#include "Script.h"
#include "Box.h"

class GameObjectFactory : public Observer
{
	/*----------MEMBER VARIABLES----------*/
public:
	std::vector<std::unique_ptr<GameObject>> GameObjectList;
	Engine & EngineHandle; 
	/*----------MEMBER FUNCTIONS----------*/
public:
	GameObjectFactory(Engine & aEngine) : EngineHandle(aEngine)
	{ GameObjectList.reserve(4096); }
	virtual ~GameObjectFactory() {};

	GameObject * SpawnGameObjectFromArchetype(const char * aFileName);
	GameObject * SpawnGameObject(Transform & aTransform = Transform());
	template <typename T> T * SpawnComponent()
	{
		Component * mComponent = nullptr;
		if (typeid(T) == typeid(Transform))
		{
			// Create root component from supplied/default transform
			mComponent = new Transform();
		}
		// Primitive components must have their registration handled by the caller
		else if (typeid(T) == typeid(Primitive))
		{
			mComponent = new Primitive();
		}
		else if (typeid(T) == typeid(Sprite))
		{
			mComponent = new Sprite();
			EngineHandle.GetRenderer().RegisterPrimitive(static_cast<Primitive *>(mComponent));
		}
		else if (typeid(T) == typeid(Mesh))
		{
			mComponent = new Mesh();
			EngineHandle.GetRenderer().RegisterPrimitive(static_cast<Primitive *>(mComponent));
		}
		else if (typeid(T) == typeid(Physics))
		{
			mComponent = new Physics();
			EngineHandle.GetPhysicsManager().RegisterPhysicsObject(static_cast<Physics *>(mComponent));
		}
		else if (typeid(T) == typeid(Controller))
		{
			mComponent = new Controller(EngineHandle.GetInputManager(), EngineHandle.GetFramerateController());
		}
		else if (typeid(T) == typeid(Script))
		{
			mComponent = new Script();
		}
		else if (typeid(T) == typeid(Box))
		{
			mComponent = new Box();
			EngineHandle.GetPhysicsManager().RegisterColliderObject(static_cast<Collider *>(mComponent));
		}
		return static_cast<T *>(mComponent);
	}
	
	virtual void OnNotify(Event * aEvent) override;

};
