#pragma once
// Eigen arbitrary size dense matrix header
#include <Eigen/Dense>
#include <vector>

// Abstract base class for any type of constraint
// A constraint is used to affect/limit an objects degree of freedom(s)
class Collider;
class Constraint
{
	/*-----------MEMBER VARIABLES-----------*/
public:
	Collider * ColliderA;
	Collider * ColliderB;

	// The constraint Jacobian is a 12-by-1 row vector
	Eigen::Matrix<float, 1, 12> Jacobian;
	// B = M^-1 * J^T (M inverse * J transpose)
	Eigen::Matrix<float, 12, 1> Catto_B;
	// M^-1 matrix (M inverse)
	Eigen::Matrix<float, 12, 12> InverseMassMatrix;

	// Lagrangian multiplier/ magnitude of impulse to resolve the constraint, in constraint space
	float ImpulseMagnitude = 0.0f;
	// 'Initial Guess' at ImpulseMagnitude
	static float InitialLambda;
	// η (or) 'Eta' value, updated every iteration of the solver
	float Catto_Eta = 0.0f;
	// ∆λi value, updated every iteration of the solver
	float DeltaLambda = 0.0f;
	// index in ConstraintObjectsList
	int ConstraintSlot = 0;
	// 'Effective' mass of constraint system
	float EffectiveMass = 0.0f;
	/*-----------MEMBER FUNCTIONS-----------*/
public:
	Constraint(Collider & aColliderA, Collider & aColliderB) :
		ColliderA(&aColliderA),
		ColliderB(&aColliderB) 
	{
		// Calculate the Inverse Mass Matrix when created
		CalculateInverseMassMatrices();
	}
	// All constraints must implement how the solver will handle them
	virtual float Solve(float aTimestep, std::vector<Eigen::Matrix<float, 6, 1>> & aCatto_A, Eigen::Matrix<float, 12, 1> & aCurrentVelocityVector, Eigen::Matrix<float, 12, 1> & aExternalForceVector) = 0;
	// All constraints have different components for their Jacobians, hence calculation is left up to child class
	virtual void CalculateJacobian() = 0;

	void CalculateInverseMassMatrices();
};