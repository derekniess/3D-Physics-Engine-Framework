#include "GameObject.h"
#include "Transform.h"
#include "Physics.h"
#include "Collider.h"
#include "Constraint.h"

float Constraint::InitialLambda = 0.0f;

void Constraint::CalculateInverseMassMatrices()
{
	// Calculate inverse mass matrices for entire constraint system
	Physics & physicsA = *(ColliderA->pOwner->GetComponent<Physics>());
	Physics & physicsB = *(ColliderB->pOwner->GetComponent<Physics>());

	// Calculate mass matrix of object A
	Eigen::Matrix3f massMatrixA;
	massMatrixA.setIdentity();
	massMatrixA *= physicsA.Mass;

	// Calculate mass matrix of object B
	Eigen::Matrix3f massMatrixB;
	massMatrixB.setIdentity();
	massMatrixB *= physicsB.Mass;

	// Convert from GLM matrices to Eigen matrices
	Eigen::Matrix3f inertiaTensorA;
	Eigen::Matrix3f inertiaTensorB;
	Eigen::Matrix3f rotationMatrixA_Eigen, rotationMatrixB_Eigen;
	glm::mat3 rotationMatrixA_GLM = glm::mat3_cast(physicsA.pOwner->GetComponent<Transform>()->Rotation);
	glm::mat3 rotationMatrixB_GLM = glm::mat3_cast(physicsB.pOwner->GetComponent<Transform>()->Rotation);

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			// Factor objects masses into inertia tensors
			inertiaTensorA(i, j) = ColliderA->InertiaTensor[i][j] * physicsA.Mass;
			inertiaTensorB(i, j) = ColliderB->InertiaTensor[i][j] * physicsB.Mass;
			rotationMatrixA_Eigen(i, j) = rotationMatrixA_GLM[i][j];
			rotationMatrixB_Eigen(i, j) = rotationMatrixB_GLM[i][j];
		}
	}

	Eigen::Matrix3f inverseInertiaTensorA = inertiaTensorA.inverse();
	Eigen::Matrix3f inverseInertiaTensorB = inertiaTensorB.inverse();
	Eigen::Matrix3f inverseMassMatrixA = massMatrixA.inverse();
	Eigen::Matrix3f inverseMassMatrixB = massMatrixB.inverse();

	// Convert from local space to world space using the 3x3 submatrix of the Rotation transform
	inverseInertiaTensorA = rotationMatrixA_Eigen * inverseInertiaTensorA;
	inverseInertiaTensorB = rotationMatrixB_Eigen * inverseInertiaTensorB;

	inverseInertiaTensorA = inverseInertiaTensorA * rotationMatrixA_Eigen.transpose();
	inverseInertiaTensorB = inverseInertiaTensorB * rotationMatrixB_Eigen.transpose();

	// Create inverse mass matrix for constraint
	InverseMassMatrix << inverseMassMatrixA, Eigen::Matrix3f::Zero(), Eigen::Matrix3f::Zero(), Eigen::Matrix3f::Zero(),
						 Eigen::Matrix3f::Zero(), inverseInertiaTensorA, Eigen::Matrix3f::Zero(), Eigen::Matrix3f::Zero(),
						 Eigen::Matrix3f::Zero(), Eigen::Matrix3f::Zero(), inverseMassMatrixB, Eigen::Matrix3f::Zero(),
						 Eigen::Matrix3f::Zero(), Eigen::Matrix3f::Zero(), Eigen::Matrix3f::Zero(), inverseInertiaTensorB;

	// Create inverse mass matrix for bodies
	ColliderA->InverseMassMatrix << inverseMassMatrixA, Eigen::Matrix3f::Zero(),
								    Eigen::Matrix3f::Zero(), inverseInertiaTensorA;

	ColliderB->InverseMassMatrix << inverseMassMatrixB, Eigen::Matrix3f::Zero(),
									Eigen::Matrix3f::Zero(), inverseInertiaTensorB;
}
