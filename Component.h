#pragma once
#include <iostream>
#include "ResourceManager.h"
#include "Observer.h"
class GameObject;
class Event;

// Component is a special type of Object that should be used for easily swappable behavior
// Components are not meant to be exist in the 'world' like Game objects do
// They only exist as attachments to an already extant Game object

class Component : public Object	// Pure virtual class 
{
	/*----------MEMBER VARIABLES----------*/
public:
	// NOTE: When adding a new component type, don't forget to add it to list of component type names in Component.cpp!
	enum ComponentType
	{
		TRANSFORM,
		PRIMITIVE,
		PHYSICS,
		CONTROLLER,
		SCRIPT,
		COLLIDER,
		LIGHT,
		TypeCount
	};
	// List of the component type names
	static const char * ComponentTypeName[ComponentType::TypeCount];
	GameObject * pOwner;
	ComponentType eComponentType;
	char * pComponentName;
	/*----------MEMBER FUNCTIONS----------*/
private:

	virtual void HandleEvent(Event *pEvent) {}
protected:	// Made protected because we don't want external visibility of constructor, but derived classes can access it
	Component(ComponentType type) : eComponentType(type) {}
public:
	virtual ~Component() {}
	virtual void Deserialize(TextFileData & aTextData) = 0;
	virtual void Serialize(TextFileData & aTextData) = 0;

	inline ComponentType GetComponentType() { return eComponentType; }
	inline GameObject * GetOwner() { return pOwner; }
	inline void SetOwner(GameObject * aOwner) { pOwner = aOwner; }

	virtual void Initialize() {};
	virtual void Destroy() {};
	virtual void Update() {};
};
