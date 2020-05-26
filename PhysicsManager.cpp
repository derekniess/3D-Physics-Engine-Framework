#include <list>

#include "PhysicsManager.h"
#include "Renderer.h"
#include "InputManager.h"
#include "FrameRateController.h"
#include "DebugFactory.h"
#include "EngineStateManager.h"

#include "GameObject.h"

#include "Transform.h"
#include "Physics.h"
#include "Collider.h"
#include "Primitive.h"
#include "ContactConstraint.h"

#include "UtilityFunctions.h"

int PhysicsManager::IntegratorIterations = 1;

void PhysicsManager::Update()
{
	/*  From 'Iterative Dynamics'
	Clearly position error will converge if and only if 0 < β < 2/∆t. If 0 < β ≤ 1/∆t then the
	position error will decay smoothly to zero. If 1/∆t < β < 2/∆t then the position
	error will decay with an oscillation. 
	*/
	//BaumgarteScalar = 1 / (2 * EngineHandle.GetFramerateController().DeltaTime);

	// Three Stages
	// Simulation : Update the state of all Physics objects
	Simulation();

	// Collision Detection : Check every Collider for collision against every other Collider
	DetectCollision();

	// Constraint Resolution: Solve all the constraints that were violated this frame using sequential impulse solver
	// http://www.bulletphysics.com/ftp/pub/test/physics/papers/IterativeDynamics.pdf
	SolveConstraints();
}

void PhysicsManager::DetectCollision()
{
	// Do collision detection for each pair of colliders
	for (int i = 0; i < ColliderObjectsList.size(); ++i)
	{
		for (int j = 0; j < ColliderObjectsList.size(); ++j)
		{
			if( i == j )
				break;
			Collider * collider1 = ColliderObjectsList[i];
			Collider * collider2 = ColliderObjectsList[j];

			Transform * transform1 = collider1->GetOwner()->GetComponent<Transform>();
			Transform * transform2 = collider2->GetOwner()->GetComponent<Transform>();

			ContactData newContactData;
			// Calculate the model matrices and store in contact data for future use
			glm::mat4 model1, model2;
			glm::mat4 translate = glm::translate(transform1->GetPosition());
			glm::mat4 rotate = glm::mat4_cast(transform1->GetRotation());
			glm::mat4 scale = glm::scale(transform1->GetScale());
			model1 = translate * rotate * scale;

			translate = glm::translate(transform2->GetPosition());
			rotate = glm::mat4_cast(transform2->GetRotation());
			scale = glm::scale(transform2->GetScale());
			model2 = translate * rotate * scale;

			newContactData.LocalToWorldMatrixA = model1;
			newContactData.LocalToWorldMatrixB = model2;

			// Set to Red if colliding, Green if not colliding
			if (GJKCollisionHandler(collider1, collider2, newContactData))
			{
				// Check if contact constraint between these two bodies already exists before adding another one
				bool bAlreadyExists = false;

				for (Constraint * constraint : ConstraintObjectsList)
				{
					ContactConstraint * contactConstraint = nullptr;
					contactConstraint = dynamic_cast<ContactConstraint *>(constraint);
					if (contactConstraint)
					{
						if (contactConstraint->ColliderA == collider1 && contactConstraint->ColliderB == collider2)
						{
							bAlreadyExists = true;
							break;
						}
					}
				}

				if (bAlreadyExists == false)
				{
					// Create a contact constraint between the two objects
					ContactConstraint * newConstraint = new ContactConstraint(*collider1, *collider2);
					newConstraint->ConstraintData = newContactData;
					newConstraint->CalculateJacobian();

					// Register it to be resolved later
					RegisterConstraintObject(newConstraint);
				}

				Primitive * mesh1 = collider1->GetOwner()->GetComponent<Primitive>();
				mesh1->SetVertexColorsUniform(glm::vec3(1.0f, 0.0f, 0.0f));

				Primitive * mesh2 = collider2->GetOwner()->GetComponent<Primitive>();
				mesh2->SetVertexColorsUniform(glm::vec3(1.0f, 0.0f, 0.0f));

				glm::vec3 endPoint = newContactData.ContactPositionA_WS + newContactData.PenetrationDepth * newContactData.Normal;

				// Render contact normal
				Arrow newDebugArrow(glm::vec3(newContactData.ContactPositionA_WS), endPoint);
				//newDebugArrow.Scale = newContactData.PenetrationDepth;
				EngineHandle.GetDebugFactory().RegisterDebugArrow(newDebugArrow);
				// Render contact point
				Quad newQuad(newContactData.ContactPositionA_WS);
				EngineHandle.GetDebugFactory().RegisterDebugQuad(newQuad);
			}
			else
			{
				Primitive * mesh1 = collider1->GetOwner()->GetComponent<Primitive>();
				mesh1->SetVertexColorsUniform(glm::vec3(0.0f, 1.0f, 0.0f));

				Primitive * mesh2 = collider2->GetOwner()->GetComponent<Primitive>();
				mesh2->SetVertexColorsUniform(glm::vec3(0.0f, 1.0f, 0.0f));
			}
		}
	}
}

// Casey Muratori explains it best: https://www.youtube.com/watch?v=Qupqu1xe7Io
bool PhysicsManager::GJKCollisionHandler(Collider * aCollider1, Collider * aCollider2, ContactData & aContactData)
{
	Physics * physics1 = aCollider1->GetOwner()->GetComponent<Physics>();
	Physics * physics2 = aCollider2->GetOwner()->GetComponent<Physics>();

	Simplex simplex;
	// Choose initial search direction as the vector from center of Object1 to the center of Object2
	glm::vec3 searchDirection = physics1->GetCurrentPosition() - physics2->GetCurrentPosition();
	// Find farthest point along search direction to get first point on the Minkowski surface, and add it to the simplex
	SupportPoint newSupportPoint = Utility::Support(aCollider1, aCollider2, searchDirection, aContactData.LocalToWorldMatrixA, aContactData.LocalToWorldMatrixB);
	simplex.Push(newSupportPoint);

	// Invert the search direction for the next point
	searchDirection *= -1.0f;

	const unsigned iterationLimit = 75;
	unsigned iterationCount = 0;

	while (true)
	{
		if (iterationCount++ >= iterationLimit) return false;

		// Add a new point to the simplex, continuing to search for origin
		SupportPoint newSupportPoint = Utility::Support(aCollider1, aCollider2, searchDirection, aContactData.LocalToWorldMatrixA, aContactData.LocalToWorldMatrixB);
		simplex.Push(newSupportPoint);

		// If projection of newly added point along the search direction has not crossed the origin,
		// the Minkowski Difference could not contain the origin, objects are not colliding
		if (glm::dot(newSupportPoint.MinkowskiHullVertex, searchDirection) <= 0.0f)
		{
			return false;
		}
		else
		{
			// Render simplex
			if (EngineHandle.GetEngineStateManager().bShouldRenderSimplex)
			{
				LineLoop simplexDebug;
				simplexDebug.AddVertex(simplex.Vertices[0].MinkowskiHullVertex);
				simplexDebug.AddVertex(simplex.Vertices[1].MinkowskiHullVertex);
				simplexDebug.AddVertex(simplex.Vertices[2].MinkowskiHullVertex);
				simplexDebug.AddVertex(simplex.Vertices[3].MinkowskiHullVertex);
				EngineHandle.GetDebugFactory().RegisterDebugLineLoop(simplexDebug);
			}
			// If the new point IS past the origin, check if the simplex contains the origin
			if (CheckIfSimplexContainsOrigin(simplex, searchDirection))
			{
				return EPAContactDetection(simplex, aCollider1, aCollider2, aContactData);
			}
		}
	}
}

// Provides the new direction to search for the point if it isn't already within the simplex
bool PhysicsManager::CheckIfSimplexContainsOrigin(Simplex & aSimplex, glm::vec3 & aSearchDirection)
{
	// Line case
	if (aSimplex.Size == 2)
	{
		// Line cannot contain the origin, only search for the direction to it 
		glm::vec3 newPointToOldPoint = aSimplex.b.MinkowskiHullVertex - aSimplex.a.MinkowskiHullVertex;
		glm::vec3 newPointToOrigin = glm::vec3(0.0f, 0.0f, 0.0f) - aSimplex.a.MinkowskiHullVertex;

		if (glm::dot(newPointToOldPoint, newPointToOrigin) > 0.0f)
		{
			// The vector from new point to old point defines the Voronoi region closest to origin
			// Get the new search direction by triple cross product, gives the normal to the edge, that points towards origin
			glm::vec3 newSearchDirection;
			newSearchDirection = Utility::TripleCrossProduct(newPointToOldPoint, newPointToOrigin, newPointToOldPoint);
			aSearchDirection = newSearchDirection;
			// No need to change the simplex, return it as [A, B]
			return false;
		}
		else
		{
			// Newly added point defines the Voronoi region closest to origin, the new point is the simplex
			aSearchDirection = newPointToOrigin;
			aSimplex.Clear();
			// Return it as [A]
			aSimplex.Push(aSimplex.a);
			return false;
		}
	}
	// Triangle case
	else if (aSimplex.Size == 3)
	{
		// Find the newly added features
		glm::vec3 newPointToOrigin = glm::vec3(0.0f, 0.0f, 0.0f) - aSimplex.a.MinkowskiHullVertex;
		glm::vec3 edge1 = aSimplex.b.MinkowskiHullVertex - aSimplex.a.MinkowskiHullVertex;
		glm::vec3 edge2 = aSimplex.c.MinkowskiHullVertex - aSimplex.a.MinkowskiHullVertex;
		// Find the normals to the triangle and the two edges
		glm::vec3 triangleNormal = glm::cross(edge1, edge2);
		glm::vec3 edge1Normal = glm::cross(edge1, triangleNormal);
		glm::vec3 edge2Normal = glm::cross(triangleNormal, edge2);

		// If origin is closer to the area along the second edge normal
		if (glm::dot(edge2Normal, newPointToOrigin) > 0.0f)
		{
			// If closer to the edge then find the normal that points towards the origin
			if (glm::dot(edge2, newPointToOrigin) > 0.0f)
			{
				aSearchDirection = Utility::TripleCrossProduct(edge2, newPointToOrigin, edge2);
				// Return it as [A, C]
				aSimplex.Clear();
				aSimplex.Set(aSimplex.a, aSimplex.c);
				return false;
			}
			// If closer to the new simplex point 
			else
			{
				// The "Star case" from the Muratori lecture
				// If new search direction should be along edge normal 
				if (glm::dot(edge1, newPointToOrigin) > 0.0f)
				{
					aSearchDirection = Utility::TripleCrossProduct(edge1, newPointToOrigin, edge1);
					// Return it as [A, B]
					aSimplex.Clear();
					aSimplex.Set(aSimplex.a, aSimplex.b);
					return false;
				}
				else // If new search direction should be along the new Simplex point
				{
					aSearchDirection = newPointToOrigin;
					// Return new simplex point alone [A]
					aSimplex.Clear();
					aSimplex.Push(aSimplex.a);
					return false;
				}
			}
		}
		else
		{
			// The "Star case" from the Muratori lecture
			// If closer to the first edge normal
			if (glm::dot(edge1Normal, newPointToOrigin) > 0.0f)
			{
				// If new search direction should be along edge normal
				if (glm::dot(edge1, newPointToOrigin) > 0.0f)
				{
					aSearchDirection = Utility::TripleCrossProduct(edge1, newPointToOrigin, edge1);
					// Return it as [A, B]
					aSimplex.Clear();
					aSimplex.Set(aSimplex.a, aSimplex.b);
					return false;
				}
				else // If new search direction should be along the new Simplex point
				{
					aSearchDirection = newPointToOrigin;
					// Return new simplex point alone [A]
					aSimplex.Clear();
					aSimplex.Push(aSimplex.a);
					return false;
				}
			}
			else
			{
				// Check if it is above the triangle
				if (glm::dot(triangleNormal, newPointToOrigin) > 0.0f)
				{
					aSearchDirection = triangleNormal;
					// No need to change the simplex, return as [A, B, C]
					return false;
				}
				else // Has to be below the triangle, all 4 other possible regions checked
				{
					aSearchDirection = -triangleNormal;
					// Return simplex as [A, C, B]
					aSimplex.Set(aSimplex.a, aSimplex.c, aSimplex.b);
					return false;
				}
			}
		}
	}
	else if (aSimplex.Size == 4)
	{
		// the simplex is a tetrahedron, must check if it is outside any of the side triangles,
		// if it is then set the simplex equal to that triangle and continue, otherwise we know
		// there is an intersection and exit

		glm::vec3 edge1 = aSimplex.b.MinkowskiHullVertex - aSimplex.a.MinkowskiHullVertex;
		glm::vec3 edge2 = aSimplex.c.MinkowskiHullVertex - aSimplex.a.MinkowskiHullVertex;
		glm::vec3 edge3 = aSimplex.d.MinkowskiHullVertex - aSimplex.a.MinkowskiHullVertex;

		glm::vec3 face1Normal = glm::cross(edge1, edge2);
		glm::vec3 face2Normal = glm::cross(edge2, edge3);
		glm::vec3 face3Normal = glm::cross(edge3, edge1);

		glm::vec3 newPointToOrigin = glm::vec3(0.0f) - aSimplex.a.MinkowskiHullVertex;

		if (glm::dot(face1Normal, newPointToOrigin) > 0.0f)
		{
			// Origin is in front of first face, simplex is correct already
			goto tag;
		}

		if (glm::dot(face2Normal, newPointToOrigin) > 0.0f)
		{
			// Origin is in front of second face, simplex is set to this triangle [A, C, D]
			aSimplex.Clear();
			aSimplex.Set(aSimplex.a, aSimplex.c, aSimplex.d);
			goto tag;
		}

		if (glm::dot(face3Normal, newPointToOrigin) > 0.0f)
		{
			// Origin is in front of third face, simplex is set to this triangle [A, D, B]
			aSimplex.Clear();
			aSimplex.Set(aSimplex.a, aSimplex.d, aSimplex.b);
			goto tag;
		}

		// If reached here it means the simplex MUST contain the origin, intersection confirmed
		return true;
	tag:
		// the simplex is equal to the triangle that the origin is in front of
		// this is exactly the same as the triangular simplex test except that we know
		// that the origin is not behind the triangle

		glm::vec3 edge1Normal = glm::cross(edge1, face1Normal);
		if (glm::dot(edge1Normal, newPointToOrigin) > 0.0f)
		{
			aSearchDirection = Utility::TripleCrossProduct(edge1, newPointToOrigin, edge1);
			// Origin is along the normal of edge1, set the simplex to that edge [A, B]
			aSimplex.Clear();
			aSimplex.Set(aSimplex.a, aSimplex.b);
			return false;
		}

		glm::vec3 edge2Normal = glm::cross(face1Normal, edge2);
		if (glm::dot(edge1Normal, newPointToOrigin) > 0.0f)
		{
			aSearchDirection = Utility::TripleCrossProduct(edge2, newPointToOrigin, edge2);
			// Origin is along the normal of edge2, set the simplex to that edge [A, C]
			aSimplex.Clear();
			aSimplex.Set(aSimplex.a, aSimplex.c);
			return false;
		}

		aSearchDirection = face1Normal;
		// If reached here the origin is along the first face normal, set the simplex to this face [A, B, C]
		aSimplex.Clear();
		aSimplex.Set(aSimplex.a, aSimplex.b, aSimplex.c);
		return false;
	}
}

void PhysicsManager::SolveConstraints()
{
	// Skip solver if no constraints
	if (ConstraintObjectsList.size() == 0)
		return;
	// TODO : [@Derek] - Figure out a better name for the temporary vectors
	std::vector<Eigen::Matrix<float, 6 , 1>> catto_A;
	catto_A.reserve(ColliderObjectsList.size());

	// Initialize one entry per collider
	for (int i = 0; i < ColliderObjectsList.size(); ++i)
	{
		catto_A.push_back(Eigen::Matrix<float, 6, 1>());
		catto_A.back().setZero();
	}
	// Calculate preliminary values of Catto_A - it is used to store already calculated impulses
	for (int i = 0; i < ConstraintObjectsList.size(); ++i)
	{
		Constraint * constraint = nullptr;
		constraint = ConstraintObjectsList[i];

		if (constraint)
		{
			// Set lambda to its initial value
			constraint->ImpulseMagnitude = constraint->InitialLambda;

			// Per constraint, the first non-static body is used to calculate the catto_A vector for that body
			if (constraint->ColliderA->eColliderType > 0)
			{
				catto_A[constraint->ColliderA->ColliderSlot] += constraint->ColliderA->InverseMassMatrix * constraint->ColliderA->ContactJacobian.transpose() * constraint->ImpulseMagnitude;
			}
			else
			{
				catto_A[constraint->ColliderB->ColliderSlot] += constraint->ColliderB->InverseMassMatrix * constraint->ColliderB->ContactJacobian.transpose() * constraint->ImpulseMagnitude;
			}
		}
	}
	// Refine the Lagrangian multiplier 'λ' using Gauss-Siedel solver
	for (int iterations = 0; iterations < 1; ++iterations)
	{
		for (int i = 0; i < ConstraintObjectsList.size(); ++i)
		{
			Constraint * constraint = nullptr;
			constraint = ConstraintObjectsList[i];
			float deltaLambda = 0.0f;
			float deltaTime = EngineHandle.GetFramerateController().DeltaTime;

			if (constraint)
			{
				Physics & physicsA = *constraint->ColliderA->pOwner->GetComponent<Physics>();
				Physics & physicsB = *constraint->ColliderB->pOwner->GetComponent<Physics>();

				// Putting the bodies quantities in a matrix form like this allows for a convenient transformation when multiplied by inverse mass matrix
				// Each quantity is multiplied against the value that corresponds to it (Inertia/angular velocity/torque and Mass/linear velocity/force)
				Eigen::Matrix<float, 12, 1> velocityVector; // Column vector
				velocityVector << physicsA.LinearVelocity.x, physicsA.LinearVelocity.y, physicsA.LinearVelocity.z,
								  physicsA.AngularVelocity.x, physicsA.AngularVelocity.y, physicsA.AngularVelocity.z,
								  physicsB.LinearVelocity.x, physicsB.LinearVelocity.y, physicsB.LinearVelocity.z,
								  physicsB.AngularVelocity.x, physicsB.AngularVelocity.y, physicsB.AngularVelocity.z;

				Eigen::Matrix<float, 12, 1> externalForceVector; // Column vector
				externalForceVector << physicsA.Force.x, physicsA.Force.y, physicsA.Force.z,
									   physicsA.Torque.x, physicsA.Torque.y, physicsA.Torque.z,
									   physicsB.Force.x, physicsB.Force.y, physicsB.Force.z,
									   physicsB.Torque.x, physicsB.Torque.y, physicsB.Torque.z;

				// Each type of constraint calls its own solver
				deltaLambda = constraint->Solve(deltaTime, catto_A, velocityVector, externalForceVector);

				// Get force of constraint for each body using the Lagrangian multiplier for magnitude and corresponding Jacobian for direction
				Eigen::Matrix<float, 6, 1> constraintForceA = constraint->ColliderA->ContactJacobian.transpose() * deltaLambda * deltaTime;
				Eigen::Matrix<float, 6, 1> constraintForceB = constraint->ColliderB->ContactJacobian.transpose() * deltaLambda * deltaTime;

				glm::vec3 forceA(constraintForceA(0), constraintForceA(1), constraintForceA(2)), torqueA(constraintForceA(3), constraintForceA(4), constraintForceA(5));
				glm::vec3 forceB(constraintForceB(0), constraintForceB(1), constraintForceB(2)), torqueB(constraintForceB(3), constraintForceB(4), constraintForceB(5));

				// Zero out forces if either object is static
				if (constraint->ColliderA->eColliderType == Collider::STATIC)
				{
					forceA *= 0;
					torqueA *= 0;
				}
				else if (constraint->ColliderB->eColliderType == Collider::STATIC)
				{
					forceB *= 0;
					torqueB *= 0;
				}

				// Create inverse mass matrices
				glm::mat3 inverseMassMatrixA = glm::mat3(1), inverseMassMatrixB = glm::mat3(1);
				inverseMassMatrixA *= physicsA.InverseMass;
				inverseMassMatrixB *= physicsB.InverseMass;

				// Create inverse inertia tensors
				glm::mat3 inverseInertiaTensorA, inverseInertiaTensorB;
				inverseInertiaTensorA = glm::inverse(constraint->ColliderA->InertiaTensor);
				inverseInertiaTensorB = glm::inverse(constraint->ColliderB->InertiaTensor);

				// Convert from local space to world space using the 3x3 submatrix of the Rotation transform
				glm::mat3 rotationMatrixA = glm::mat3_cast(physicsA.pOwner->GetComponent<Transform>()->Rotation);
				glm::mat3 rotationMatrixB = glm::mat3_cast(physicsB.pOwner->GetComponent<Transform>()->Rotation);
				inverseInertiaTensorA = rotationMatrixA * inverseInertiaTensorA * glm::transpose(rotationMatrixA);
				inverseInertiaTensorB = rotationMatrixB * inverseInertiaTensorB *  glm::transpose(rotationMatrixB);

				physicsA.LinearVelocity += inverseMassMatrixA * forceA;
				physicsA.AngularVelocity += inverseInertiaTensorA * torqueA;

				physicsB.LinearVelocity += inverseMassMatrixB * forceB;
				physicsB.AngularVelocity += inverseInertiaTensorB * torqueB;
			}
		}
	}
	ConstraintObjectsList.empty();
}

// Based on the Expanding Polytope Algorithm (EPA) as described here: http://allenchou.net/2013/12/game-physics-contact-generation-epa/
bool PhysicsManager::EPAContactDetection(Simplex & aSimplex, Collider * aShape1, Collider * aShape2, ContactData & aContactData)
{
	const float exitThreshold = 0.001f;
	const unsigned iterationLimit = 50;
	unsigned iterationCount = 0;

	std::list<PolytopeFace> polytopeFaces;
	std::list<PolytopeEdge> polytopeEdges;

	// Add all faces of simplex to the polytope
	polytopeFaces.emplace_back(aSimplex.a, aSimplex.b, aSimplex.c);
	polytopeFaces.emplace_back(aSimplex.a, aSimplex.c, aSimplex.d);
	polytopeFaces.emplace_back(aSimplex.a, aSimplex.d, aSimplex.b);
	polytopeFaces.emplace_back(aSimplex.b, aSimplex.d, aSimplex.c);
	while (true)
	{
		if (iterationCount++ >= iterationLimit)
		{
			return false;
		}
		// Find the closest face to origin (i.e. projection of any vertex along its face normal with the least value)
		float minimumDistance = FLT_MAX;
		std::list<PolytopeFace>::iterator closestFace = polytopeFaces.begin();
		for (auto iterator = polytopeFaces.begin(); iterator != polytopeFaces.end(); ++iterator)
		{
			float distance = glm::dot(iterator->FaceNormal, iterator->Points[0].MinkowskiHullVertex);
			if (distance < minimumDistance)
			{
				minimumDistance = distance;
				closestFace = iterator;
			}
		}
		// With the closest face now known, find new support point on the Minkowski Hull using normal to that face
		SupportPoint newPolytopePoint = Utility::Support(aShape1, aShape2, closestFace->FaceNormal, aContactData.LocalToWorldMatrixA, aContactData.LocalToWorldMatrixB);

		// If this new point is within a tolerable limit of the origin, 
		// assume we have found the closest triangle on the Minkowski Hull to the origin
		if (glm::dot(closestFace->FaceNormal, newPolytopePoint.MinkowskiHullVertex) - minimumDistance < exitThreshold)
		{
			LineLoop closestFaceObjectA;
			closestFaceObjectA.Color = glm::vec4(1, 1, 0, 1);
			closestFaceObjectA.AddVertex(closestFace->Points[0].World_SupportPointA);
			closestFaceObjectA.AddVertex(closestFace->Points[1].World_SupportPointA);
			closestFaceObjectA.AddVertex(closestFace->Points[2].World_SupportPointA);

			LineLoop closestFaceObjectB;
			closestFaceObjectB.Color = glm::vec4(0, 1, 1, 1);
			closestFaceObjectB.AddVertex(closestFace->Points[0].World_SupportPointB);
			closestFaceObjectB.AddVertex(closestFace->Points[1].World_SupportPointB);
			closestFaceObjectB.AddVertex(closestFace->Points[2].World_SupportPointB);

			LineLoop closestPolytopeFace;
			closestPolytopeFace.Color = glm::vec4(1, 0, 1, 1);
			closestPolytopeFace.AddVertex(closestFace->Points[0].MinkowskiHullVertex);
			closestPolytopeFace.AddVertex(closestFace->Points[1].MinkowskiHullVertex);
			closestPolytopeFace.AddVertex(closestFace->Points[2].MinkowskiHullVertex);

			EngineHandle.GetDebugFactory().RegisterDebugLineLoop(closestFaceObjectA);
			EngineHandle.GetDebugFactory().RegisterDebugLineLoop(closestFaceObjectB);
			EngineHandle.GetDebugFactory().RegisterDebugLineLoop(closestPolytopeFace);

			return ExtrapolateContactInformation(&(*closestFace), aContactData);
		}

		// Otherwise, check what faces can be 'seen' from the newly added support point and delete them from the polytope
		for (auto iterator = polytopeFaces.begin(); iterator != polytopeFaces.end();)
		{
			// A face is considered as "seen" if the new support point is on the positive halfspace of the plane defined by it
			glm::vec3 planeVector = newPolytopePoint.MinkowskiHullVertex - iterator->Points[0].MinkowskiHullVertex;

			if (glm::dot(iterator->FaceNormal, planeVector) > 0.0f)
			{
				// Only adds the outside edges of the 'seen' face, and ignores the others
				Utility::AddEdge(polytopeEdges, iterator->Points[0], iterator->Points[1]);
				Utility::AddEdge(polytopeEdges, iterator->Points[1], iterator->Points[2]);
				Utility::AddEdge(polytopeEdges, iterator->Points[2], iterator->Points[0]);
				// Remove the face from the Polytope
				iterator = polytopeFaces.erase(iterator);
				continue;
			}
			++iterator;
		}

		// Create new polytope faces using the new support point from the valid edges in the edge list
		for (auto iterator = polytopeEdges.begin(); iterator != polytopeEdges.end(); ++iterator)
		{
			polytopeFaces.emplace_back(newPolytopePoint, iterator->Points[0], iterator->Points[1]);
		}

		// Clear the edge list every iteration of the expansion
		polytopeEdges.clear();
	}
}

// By using the closest face of the Minkowski hull to the origin in Minkowski space, 
// you obtain the closest face to the penetration in world space.
// Barycentric projection allows you to save the contribution to a point 
// in a triangle between its vertices, and you can use that to obtain the contact point in world space
bool PhysicsManager::ExtrapolateContactInformation(PolytopeFace * aClosestFace, ContactData & aContactData)
{
	const float distanceFromOrigin = glm::dot(aClosestFace->FaceNormal, aClosestFace->Points[0].MinkowskiHullVertex);

	// calculate the barycentric coordinates of the closest triangle with respect to
	// the projection of the origin onto the triangle
	float bary_u, bary_v, bary_w;
	Utility::BarycentricProjection(aClosestFace->FaceNormal * distanceFromOrigin, aClosestFace->Points[0].MinkowskiHullVertex, aClosestFace->Points[1].MinkowskiHullVertex, aClosestFace->Points[2].MinkowskiHullVertex, bary_u, bary_v, bary_w);
	
	// if any of the barycentric coefficients have a magnitude greater than 1 or lesser than and equal to 0, then the origin is not within the triangular prism described by 'triangle'
	// thus, there is no collision here, return false
	if (fabs(bary_u) > 1.0f || fabs(bary_v) > 1.0f || fabs(bary_w) > 1.0f)
		return false;
	if (bary_u <= 0.0f || bary_v <= 0.0f || bary_w <= 0.0f)
		return false;

	// A Contact points
	glm::vec3 supportLocal1 = aClosestFace->Points[0].Local_SupportPointA;
	glm::vec3 supportLocal2 = aClosestFace->Points[1].Local_SupportPointA;
	glm::vec3 supportLocal3 = aClosestFace->Points[2].Local_SupportPointA;
	// Contact point on object A in local space
	aContactData.ContactPositionA_LS = (bary_u * supportLocal1) + (bary_v * supportLocal2) + (bary_w * supportLocal3);
	// Contact point on object A in world space
	aContactData.ContactPositionA_WS = aContactData.LocalToWorldMatrixA * glm::vec4(aContactData.ContactPositionA_LS, 1);

	// B contact points
	supportLocal1 = aClosestFace->Points[0].Local_SupportPointB;
	supportLocal2 = aClosestFace->Points[1].Local_SupportPointB;
	supportLocal3 = aClosestFace->Points[2].Local_SupportPointB;
	// Contact point on object B in local space
	aContactData.ContactPositionB_LS = (bary_u * supportLocal1) + (bary_v * supportLocal2) + (bary_w * supportLocal3);
	// Contact point on object B in world space
	aContactData.ContactPositionB_WS = aContactData.LocalToWorldMatrixB * glm::vec4(aContactData.ContactPositionB_LS, 1);

	// Contact normal
	aContactData.Normal = glm::normalize(aClosestFace->FaceNormal);
	// Penetration depth
	aContactData.PenetrationDepth = distanceFromOrigin;

	return true;
}



void PhysicsManager::OnNotify(Event * aEvent)
{
	// Check if this is an Engine event
	EngineEvent * engineEvent = nullptr;
	engineEvent = dynamic_cast<EngineEvent *>(aEvent);

	if (engineEvent)
	{
		switch (engineEvent->EventID)
		{
			case EngineEvent::EventList::ENGINE_INIT:
			{
			}
			case EngineEvent::EventList::ENGINE_TICK:
			{
				Update();
			}
		return;
		}
	}

}

void PhysicsManager::RegisterPhysicsObject(Physics * aNewPhysics)
{
	PhysicsObjectsList.push_back(aNewPhysics);
}

void PhysicsManager::RegisterColliderObject(Collider * aNewCollider)
{
	aNewCollider->ColliderSlot = (int)ColliderObjectsList.size();
	ColliderObjectsList.push_back(aNewCollider);
}

void PhysicsManager::RegisterConstraintObject(Constraint * aNewConstraint)
{
	aNewConstraint->ConstraintSlot = (int)ConstraintObjectsList.size();
	ConstraintObjectsList.push_back(aNewConstraint);
}

void PhysicsManager::Simulation()
{
	Physics * pSimulation1 = nullptr, * pSimulation2 = nullptr;
	float deltatime = EngineHandle.GetFramerateController().DeltaTime;
	if (!bShouldSimulate)
		return;
	// Integration
	for (int i = 0; i < IntegratorIterations; ++i)
	{
		for (auto iterator = PhysicsObjectsList.begin(); iterator != PhysicsObjectsList.end(); ++iterator)
		{
			pSimulation1 = static_cast<Physics *>(*iterator);
			// Updates physics component with current transform values
			pSimulation1->SyncPhysicsWithTransform();

			if (EngineHandle.GetInputManager().isKeyPressed(GLFW_KEY_1))
			{
				pSimulation1->IntegratePositionVerlet(deltatime);
			}
			else if (EngineHandle.GetInputManager().isKeyPressed(GLFW_KEY_2))
			{
				pSimulation1->IntegrateRK4(EngineHandle.GetFramerateController().TotalTime, deltatime);
			}
			else
			{
				// Default type of integration is Euler
				pSimulation1->IntegrateEuler(deltatime);
			}
		}
	}
}
