#pragma once
// Eigen arbitrary size dense matrix header
#include <Eigen/Dense>

#include "Component.h"
#include "DebugVertex.h"

// Abstract base class for any component that implements a collision shape (box, capsule, sphere, etc) 
class Collider : public Component
{
	/*-----------MEMBER VARIABLES-----------*/
public:
	enum ColliderType
	{
		STATIC,
		DYNAMIC,
		KINEMATIC,
		ColliderTypeCount
	};
	bool bIsCollisionEnabled;
	bool bShouldRenderDebug;
	std::vector<DebugVertex> Vertices;
	// index in ColliderObjectsList
	int ColliderSlot = 0;
	ColliderType eColliderType = ColliderType::DYNAMIC;
	// Jacobians per constraint type
	Eigen::Matrix<float, 1, 6> ContactJacobian;
	// Rotational Inertia Tensor
	glm::mat3 InertiaTensor = glm::mat3(1);
	// Contains inverted mass and inertia tensors
	Eigen::Matrix<float, 6, 6> InverseMassMatrix;
	/*-----------MEMBER FUNCTIONS-----------*/
public:
	Collider() : 
		Component(Component::COLLIDER) {}
	~Collider() {}

	static inline ComponentType GetComponentID() { return Component::ComponentType::COLLIDER; }

	virtual glm::vec3 FindFarthestPointInDirection(glm::vec3 aDirection) = 0;
	virtual void Update() {};
	virtual void Deserialize(TextFileData aTextData) {};

};