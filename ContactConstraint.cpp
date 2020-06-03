#include <algorithm>
#include "ContactConstraint.h"
#include "Engine.h"
#include "PhysicsManager.h"
#include "PhysicsUtilities.h"
#include "GameObject.h"
#include "Transform.h"
#include "Physics.h"
#include "Collider.h"

void ContactConstraint::CalculateJacobian()
{
	glm::vec3 & contactNormal = ConstraintData.Normal;
	
	glm::vec3 & centerOfMassA = ColliderA->pOwner->GetComponent<Transform>()->GetPosition();
	glm::vec3 momentArmA = ConstraintData.ContactPositionA_WS - centerOfMassA;
	
	glm::vec3 & centerOfMassB = ColliderB->pOwner->GetComponent<Transform>()->GetPosition();
	glm::vec3 momentArmB = ConstraintData.ContactPositionB_WS - centerOfMassB;

	glm::vec3 crossProductA = glm::cross(momentArmA, contactNormal);
	glm::vec3 crossProductB = glm::cross(momentArmB, contactNormal);

	// Create the individual collider jacobians
	ColliderA->ContactJacobian << -contactNormal.x, -contactNormal.y, -contactNormal.z,
								  -crossProductA.x, -crossProductA.y, -crossProductA.z;	
	
	ColliderB->ContactJacobian << contactNormal.x, contactNormal.y, contactNormal.z,
								  crossProductB.x, crossProductB.y, crossProductB.z;

	// By convention, if a constraint is between a dynamic and static object, then Jacobian of the static object is 0
	if (ColliderA->eColliderType == Collider::STATIC)
	{
		ColliderA->ContactJacobian *= 0.0f;
	}
	else if (ColliderB->eColliderType == Collider::STATIC)
	{
		ColliderB->ContactJacobian *= 0.0f;
	}

	// Creates constraint Jacobian from individual jacobians
	Jacobian << ColliderA->ContactJacobian, ColliderB->ContactJacobian;

	// Column vector obtained from multiplying Inverse Mass Matrix and Jacobian Transpose
	Catto_B = InverseMassMatrix * Jacobian.transpose();

	/* -- Calculating di -- */
	EffectiveMass = Jacobian * Catto_B;
}

float ContactConstraint::Solve(float aTimestep, std::vector<Eigen::Matrix<float, 6, 1>> & aCatto_A, Eigen::Matrix<float, 12, 1> & aCurrentVelocityVector, Eigen::Matrix<float, 12, 1> & aExternalForceVector)
{
	float constraintError = 0.0f; 

	/* -- Calculate constraint error from position constraint equation Cn = (x2 +r2 −x1 −r1) * n1 -- */
	glm::vec3 & centerOfMassA = ColliderA->pOwner->GetComponent<Transform>()->GetPosition();
	glm::vec3 momentArmA = ConstraintData.ContactPositionA_WS - centerOfMassA;

	glm::vec3 & centerOfMassB = ColliderB->pOwner->GetComponent<Transform>()->GetPosition();
	glm::vec3 momentArmB = ConstraintData.ContactPositionB_WS - centerOfMassB;

	glm::vec3 pointA = centerOfMassA + momentArmA;
	glm::vec3 pointB = centerOfMassB + momentArmB;
	constraintError = glm::dot((pointB - pointA), ConstraintData.Normal);

	/* -- Calculating Bias values in order to get constraint force to do work (in this case to resolve penetration) == */
	float baumgarteScalar = ColliderA->pOwner->EngineHandle.GetPhysicsManager().BaumgarteScalar;
	float penetrationSlop = ColliderA->pOwner->EngineHandle.GetPhysicsManager().PenetrationSlop;
	float restitutionSlop = ColliderA->pOwner->EngineHandle.GetPhysicsManager().RestitutionSlop;
	// Allow for objects to penetrate a bit before actually applying Baumgarte stabilization
	constraintError = std::max(constraintError - penetrationSlop, 0.0f);

	glm::vec3 linearVelocityA = ColliderA->pOwner->GetComponent<Physics>()->CurrentLinearVelocity;
	glm::vec3 linearVelocityB = ColliderB->pOwner->GetComponent<Physics>()->CurrentLinearVelocity;
	glm::vec3 angularVelocityA = ColliderA->pOwner->GetComponent<Physics>()->CurrentAngularVelocity;
	glm::vec3 angularVelocityB = ColliderB->pOwner->GetComponent<Physics>()->CurrentAngularVelocity;

	glm::vec3 relativeVelocityA = linearVelocityA + glm::cross(angularVelocityA, momentArmA);
	glm::vec3 relativeVelocityB = linearVelocityB + glm::cross(angularVelocityB, momentArmB);
	glm::vec3 relativeVelocity = relativeVelocityB - relativeVelocityA;

	float projection = glm::dot(relativeVelocity, ConstraintData.Normal);
	// If relative velocity is separating the objects, constraint is solved
	if (projection > 0.0f)
	{
		DeltaLambda = 0.0f;
		return DeltaLambda;
	}
	// Allow for restitution slop as a tolerance of the relative speed
	//projection = std::max(projection - restitutionSlop, 0.0f);

	float restitutionA = ColliderA->Restitution;
	float restitutionB = ColliderB->Restitution;
	float restitution = (restitutionA + restitutionB) / 2.0f;

	float restitutionTerm = (projection * restitution);
	float baumgarteTerm = (-baumgarteScalar * constraintError) / aTimestep;
	float biasTerm = (baumgarteTerm + restitutionTerm) / aTimestep;

	/* -- Calculating 'Catto_eta' values - Equation 35 in 'Iterative Dynamics' -- */
	// V1
	float currentVelocityTerm = (Jacobian * (aCurrentVelocityVector / aTimestep));
	// Fext
	float externalForceTerm = (Jacobian * (InverseMassMatrix * aExternalForceVector));

	Catto_Eta = biasTerm + currentVelocityTerm + externalForceTerm;

	/* -- Calculating ∆λi -- */
	float bodyATerm = ColliderA->ContactJacobian * aCatto_A[ColliderA->ColliderSlot];
	float bodyBTerm = ColliderB->ContactJacobian * aCatto_A[ColliderB->ColliderSlot];

	DeltaLambda = -(Catto_Eta - bodyATerm - bodyBTerm);
	DeltaLambda /= EffectiveMass;

	float normalImpulseSumCopy = NormalImpulseSum;

	NormalImpulseSum += DeltaLambda;
	// Clamps normal impulse between 0 and positive infinity
	NormalImpulseSum = std::max(0.0f, std::min(NormalImpulseSum, FLT_MAX));

	DeltaLambda = NormalImpulseSum - normalImpulseSumCopy;

	// Calculate individual Catto_B vectors
	Eigen::Matrix<float, 6, 1> catto_B_BodyA, catto_B_BodyB;

	catto_B_BodyA << Catto_B(0), Catto_B(1), Catto_B(2),
					 Catto_B(3), Catto_B(4), Catto_B(5);

	catto_B_BodyB << Catto_B(6), Catto_B(7), Catto_B(8),
					 Catto_B(9), Catto_B(10), Catto_B(11);

	// Update catto_A values of each body
	aCatto_A[ColliderA->ColliderSlot] += DeltaLambda * catto_B_BodyA;
	aCatto_A[ColliderB->ColliderSlot] += DeltaLambda * catto_B_BodyB;

	return DeltaLambda;
}
