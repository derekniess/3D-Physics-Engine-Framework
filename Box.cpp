
#include "Typedefs.h"
#include "Box.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "Physics.h"
#include "Transform.h"
#include "Engine.h"
#include "UtilityFunctions.h"
#include "MathUtilities.h"
#include "glm\gtx\quaternion.hpp"

void Box::Initialize()
{
	// Loads the cube vertex positions/color data from file
	Vertices = std::move(pOwner->EngineHandle.GetResourceManager().ImportColliderData(std::string("Cube.fbx")));
	// Creates the inertia tensor, takes into account the scale from transform
	Transform & transform = *pOwner->GetComponent<Transform>();
	float width = Side * transform.Scale.x;
	float height = Side * transform.Scale.y;
	float depth = Side * transform.Scale.z;

	HalfSize.x = Side / 2;
	HalfSize.y = Side / 2;
	HalfSize.z = Side / 2;

	// Initialize inertia tensor of a symmetric rectangular prism in body space:
	// http://www-robotics.cs.umass.edu/~grupen/603/slides/DynamicsI.pdf
	InertiaTensor[0][0] = (1.0f / 12) * (powf(depth, 2) + powf(height, 2));
	InertiaTensor[0][1] = 0;
	InertiaTensor[0][2] = 0;
	
	InertiaTensor[1][0] = 0;
	InertiaTensor[1][1] = (1.0f / 12) * (powf(width, 2) + powf(height, 2));
	InertiaTensor[1][2] = 0;
	
	InertiaTensor[2][0] = 0;
	InertiaTensor[2][1] = 0;
	InertiaTensor[2][2] = (1.0f / 12) * (powf(depth, 2) + powf(width, 2));
}

glm::vec3 Box::FindFarthestPointInDirection(vector3 aDirection)
{
	Transform * transform = pOwner->GetComponent<Transform>();

	// To deal with rotations, convert the direction to local space before performing search
	aDirection = glm::rotate(glm::inverse(transform->Rotation), vector4(aDirection, 0));

	// 'Extents' Method taken from Lattice3D engine - NOTE: causes more false positives AND can cause degenerate simplexes somehow
	// https://bitbucket.org/Hacktank/lattice3d/src
	vector3 extentInDirection(SIGN(aDirection.x) * HalfSize.x, SIGN(aDirection.y) * HalfSize.y, SIGN(aDirection.z) * HalfSize.z);

	vector3 result = extentInDirection;
	return result;
}
