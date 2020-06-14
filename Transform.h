#pragma once
#include "Component.h"
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <iterator>
#include <cassert>
#include <sstream>
#include "Typedefs.h"
// Component header files
#include "Component.h"

class Transform : public Component
{
	/*----------MEMBER VARIABLES----------*/
public:
	vector3 Position;			
	quaternion Rotation;			
	vector3 Scale;
	/*----------MEMBER FUNCTIONS----------*/
public:
	Transform() : Component(ComponentType::TRANSFORM), 
		Position(vector3(0)),
		Rotation(quaternion()),
		Scale(vector3(1)) 
	{}
	Transform(Transform const & CopyTransform) : Component(ComponentType::TRANSFORM) 
	{ 
		Position = CopyTransform.Position;
		Rotation = CopyTransform.Rotation;
		Scale = CopyTransform.Scale;
	}
	virtual ~Transform() {};

	static inline Component::ComponentType GetComponentID() { return (ComponentType::TRANSFORM); }
	static inline const char * GetComponentName() { return ComponentTypeName[ComponentType::TRANSFORM]; }

	inline vector3 GetPosition() { return Position; }
	inline quaternion GetRotation() { return Rotation; }
	inline vector3 GetScale() { return Scale; }

	inline void SetPosition(vector3 newPosition) { Position = newPosition; }
	inline void SetRotation(quaternion newRotation) { Rotation = newRotation; }
	inline void SetScale(vector3 newScale) { Scale = newScale; }
	// Rotates this transform using the provided quaternion
	inline void Rotate(quaternion aQuat) { 
		Rotation = Rotation * aQuat; 
	}

	virtual void Deserialize(TextFileData & aTextFileData) override;
	virtual void Serialize(TextFileData & aTextData) override {};
	virtual void Update() override;

};

