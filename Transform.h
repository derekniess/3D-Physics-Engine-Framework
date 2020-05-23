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
// GLM Math header files
#include <glm/gtc/quaternion.hpp>
#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>
// Component header files
#include "Component.h"

class Transform : public Component
{
	/*----------MEMBER VARIABLES----------*/
public:
	glm::vec3 Position;			
	glm::quat Rotation;			
	glm::vec3 Scale;			
	/*----------MEMBER FUNCTIONS----------*/
public:
	Transform() : Component(ComponentType::TRANSFORM), 
		Position(glm::vec3(0)),
		Rotation(glm::quat()),
		Scale(glm::vec3(1)) 
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

	inline glm::vec3 GetPosition() { return Position; }
	inline glm::quat GetRotation() { return Rotation; }
	inline glm::vec3 GetScale() { return Scale; }

	inline void SetPosition(glm::vec3 newPosition) { Position = newPosition; }
	inline void SetRotation(glm::quat newRotation) { Rotation = newRotation; }
	inline void SetScale(glm::vec3 newScale) { Scale = newScale; }
	// Rotates this transform using the provided quaternion
	inline void Rotate(glm::quat aQuat) { 
		Rotation = Rotation * aQuat; 
	}

	virtual void Deserialize(TextFileData aTextFileData) override;
	virtual void Update() override;

};

