#pragma once
#include "Typedefs.h"
#include "Component.h"

class Physics : public Component
{
public:
	/* -------- VARIABLES ---------- */
	struct Derivative
	{
		vector3 mDerivedVelocity;
		vector3 mDerivedAcceleration;
		Derivative()
		{
			mDerivedAcceleration= vector3();
			mDerivedVelocity = vector3();
		}
	};
	vector3 NextPosition = vector3();
	vector3 CurrentPosition = vector3();
	vector3 PreviousPosition = vector3();

	vector3 CurrentLinearVelocity = vector3();
	vector3 PreviousLinearVelocity = vector3();

	vector3 CurrentAngularVelocity = vector3();
	vector3 PreviousAngularVelocity = vector3();

	vector3 Force = vector3();
	vector3 Torque = vector3();

	float Mass = 1.0f;
	float InverseMass = 1.0f/Mass;

	bool bShouldGravityAffect = true;
	float GravityMagnitude = -0.8f;
	/* -------- FUNCTIONS ---------- */
	
	Physics() : Component(Component::PHYSICS)
	{}
	
	static inline Component::ComponentType GetComponentID() { return (ComponentType::PHYSICS); }
	static inline const char * GetComponentName() { return ComponentTypeName[ComponentType::PHYSICS]; }

	// GETTERS
	inline float GetMass() { return Mass; }
	inline vector3 GetCurrentPosition() { return CurrentPosition; }
	inline vector3 GetVelocity() { return CurrentLinearVelocity; }
	// SETTERS
	inline void SetMass(float mass) { Mass = mass; InverseMass = 1 / Mass; }
	inline void SetCurrentPosition(vector3 position) { CurrentPosition = position; }
	inline void SetNextPosition(vector3 position) { NextPosition = position; }
	inline void ApplyForce(vector3 newForce) { Force += newForce; }

	virtual void Initialize() override;
	virtual void Deserialize(TextFileData & aTextFileData) override {};
	virtual void Serialize(TextFileData & aTextFileData) override {};

	// Used at start of frame to sync physics with transforms (for updates from editor)
	void SyncPhysicsWithTransform();
	// Used at end of frame to sync transform to updated physics values
	void UpdateTransform();

	void IntegrateRK4(float totalTime, float dt);
	Derivative Evaluate(float t, float dt, const Derivative &);

	void IntegrateEuler(float dt);
	void IntegratePositionVerlet(float dt);
};