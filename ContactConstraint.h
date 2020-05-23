#pragma once
#include <Eigen/Dense>

#include "Constraint.h"
#include "PhysicsUtilities.h"

// Constraint used to prevent two objects from geometric interference (collision)
// http://allenchou.net/2013/12/game-physics-resolution-contact-constraints/
class ContactConstraint : public Constraint
{
	/*-----------MEMBER VARIABLES-----------*/
public:
	ContactData ConstraintData;

	// Used for clamping
	float NormalImpulseSum = 0.0f;
	float TangentImpulseSum1 = 0.0f;
	float TangentImpulseSum2 = 0.0f;
	/*-----------MEMBER FUNCTIONS-----------*/
public:
	ContactConstraint(Collider & aColliderA, Collider & aColliderB) : Constraint(aColliderA, aColliderB)
	{}
	virtual void CalculateJacobian() override;
	virtual float Solve(float aTimestep, std::vector<Eigen::Matrix<float, 6, 1>> & aCatto_A, Eigen::Matrix<float, 12, 1> & aVelocityVector, Eigen::Matrix<float, 12, 1> & aExternalForceVector) override; 

};