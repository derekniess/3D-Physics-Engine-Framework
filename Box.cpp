#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include "Box.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "Physics.h"
#include "Transform.h"
#include "Engine.h"
#include "UtilityFunctions.h"

void Box::Initialize()
{
	// Loads the cube vertex positions/color data from file
	Vertices = std::move(pOwner->EngineHandle.GetResourceManager().ImportColliderData(std::string("Cube.fbx")));
	// Creates the inertia tensor, takes into account the scale from transform
	Transform & transform = *pOwner->GetComponent<Transform>();
	float width = Side * transform.Scale.x;
	float height = Side * transform.Scale.y;
	float length = Side * transform.Scale.z;

	// Initialize inertia tensor of a symmetric rectangular prism in body space:
	// http://www-robotics.cs.umass.edu/~grupen/603/slides/DynamicsI.pdf
	InertiaTensor[0][0] = (1.0f / 12) * (powf(length, 2) + powf(height, 2));
	InertiaTensor[0][1] = 0;
	InertiaTensor[0][2] = 0;
	
	InertiaTensor[1][0] = 0;
	InertiaTensor[1][1] = (1.0f / 12) * (powf(width, 2) + powf(height, 2));
	InertiaTensor[1][2] = 0;
	
	InertiaTensor[2][0] = 0;
	InertiaTensor[2][1] = 0;
	InertiaTensor[2][2] = (1.0f / 12) * (powf(length, 2) + powf(width, 2));
}

glm::vec3 Box::FindFarthestPointInDirection(glm::vec3 aDirection)
{
	Transform * transform = pOwner->GetComponent<Transform>();

	// To deal with rotations, convert the direction to local space before performing search
	aDirection = glm::normalize(Utility::RotateVectorByQuaternion(aDirection, transform->Rotation));

	int index = 0;
	float maxiumum = glm::dot(aDirection, Vertices[0].Position);
	for (int i = 1; i < Vertices.size(); ++i)
	{
		float PositionProjectedAlongDirection = glm::dot(aDirection, Vertices[i].Position);
		if (PositionProjectedAlongDirection > maxiumum)
		{
			maxiumum = PositionProjectedAlongDirection;
			index = i;
		}
	}
	return Vertices[index].Position;
}
