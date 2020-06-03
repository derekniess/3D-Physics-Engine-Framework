#include "Physics.h"
#include "Transform.h"
#include "GameObject.h"
#include "Controller.h"
#include "Collider.h"

void Physics::Initialize()
{
}

void Physics::SyncPhysicsWithTransform()
{
	Transform * transform = this->GetOwner()->GetComponent<Transform>();
	CurrentPosition = transform->Position;
}

void Physics::UpdateTransform()
{
	// Update owner if it isn't being controlled. Controller updates physics directly
	Controller * controller = nullptr;
	controller = this->GetOwner()->GetComponent<Controller>();
	if (controller)
		return;

	Transform * transform = this->GetOwner()->GetComponent<Transform>();
	transform->Position = CurrentPosition;

	// Rotation is updated directly in the integration step
}

Physics::Derivative Physics::Evaluate(float t, float dt, const Derivative & d)
{
	glm::vec3 positionTemp, velocityTemp;

	positionTemp = CurrentPosition;
	positionTemp.x += d.mDerivedVelocity.x * dt;
	positionTemp.y += d.mDerivedVelocity.y * dt;
	positionTemp.z += d.mDerivedVelocity.z * dt;

	velocityTemp = CurrentLinearVelocity;
	velocityTemp.x += d.mDerivedAcceleration.x * dt;
	velocityTemp.y += d.mDerivedAcceleration.y * dt;
	velocityTemp.z += d.mDerivedAcceleration.z * dt;

	Derivative output;
	output.mDerivedVelocity = velocityTemp;
	output.mDerivedAcceleration = glm::vec3(0);
	return output;
}

void Physics::IntegrateEuler(float dt)
{
	Collider * collider = pOwner->GetComponent<Collider>();
	if (collider)
	{
		// Don't integrate static objects
		// TODO : [@Derek] - Separate collider and physics dependencies so that a collider doesn't need a physics component on the owner object
		if (collider->eColliderType == Collider::STATIC)
			return;
	}
	// Apply gravity
	if (bShouldGravityAffect)
		CurrentLinearVelocity += glm::vec3(0, GravityMagnitude, 0) * dt;

	PreviousLinearVelocity = CurrentLinearVelocity;
	// Integrate force and linear velocity
	CurrentLinearVelocity = CurrentLinearVelocity + ((Force * InverseMass) * dt);
	PreviousPosition = CurrentPosition;
	CurrentPosition = PreviousPosition + CurrentLinearVelocity * dt;

	PreviousAngularVelocity = CurrentAngularVelocity;
	// Integrate torque and angular velocity
	glm::vec3 axis = CurrentAngularVelocity;
	float length = glm::length(CurrentAngularVelocity);
	float angle = length * dt;
	// Prevents degenerate quaternions
	if (angle != 0.0f)
	{
		// Normalize axis
		axis = axis / length;
		glm::quat rotationDelta(std::cos(angle / 2.0f), axis * std::sin(angle / 2.0f));

		// Update rotation directly
		Transform & transform = *(pOwner->GetComponent<Transform>());
		transform.Rotation = rotationDelta * transform.Rotation;
	}
	UpdateTransform();
}

void Physics::IntegrateRK4(float totalTime, float dt)
{
	Derivative a, b, c, d;
	PreviousPosition = CurrentPosition;
	// Finds 4 derivatives
	a = Evaluate(totalTime, 0.0f, Derivative());
	b = Evaluate(totalTime, dt*0.5f, a);
	c = Evaluate(totalTime, dt*0.5f, b);
	d = Evaluate(totalTime, dt, c);
	// Finds weighted sum of derivatives for each:

	// Position from velocity
	float dxdt = 1.0f / 6.0f *
		(a.mDerivedVelocity.x + 2.0f*(b.mDerivedVelocity.x + c.mDerivedVelocity.x) + d.mDerivedVelocity.x);

	float dydt = 1.0f / 6.0f *
		(a.mDerivedVelocity.y + 2.0f*(b.mDerivedVelocity.y + c.mDerivedVelocity.y) + d.mDerivedVelocity.y);

	float dzdt = 1.0f / 6.0f *
		(a.mDerivedVelocity.z + 2.0f*(b.mDerivedVelocity.z + c.mDerivedVelocity.z) + d.mDerivedVelocity.z);
	// Velocity from acceleration
	float dvxdt = 1.0f / 6.0f *
		(a.mDerivedAcceleration.x + 2.0f*(b.mDerivedAcceleration.x + c.mDerivedAcceleration.x) + d.mDerivedAcceleration.x);

	float dvydt = 1.0f / 6.0f *
		(a.mDerivedAcceleration.y + 2.0f*(b.mDerivedAcceleration.y + c.mDerivedAcceleration.y) + d.mDerivedAcceleration.y);

	float dvzdt = 1.0f / 6.0f *
		(a.mDerivedAcceleration.z + 2.0f*(b.mDerivedAcceleration.z + c.mDerivedAcceleration.z) + d.mDerivedAcceleration.z);

	// Change position and velocity accordingly
	CurrentPosition.x += dxdt * dt;
	CurrentPosition.y += dydt * dt;
	CurrentPosition.z += dzdt * dt;

	CurrentLinearVelocity.x += dvxdt * dt;
	CurrentLinearVelocity.y += dvydt * dt;
	CurrentLinearVelocity.z += dvzdt * dt;

	UpdateTransform();
}

void Physics::IntegratePositionVerlet(float dt)
{
	NextPosition += (CurrentPosition - PreviousPosition) + (Force / Mass) * dt * dt;
	PreviousPosition = CurrentPosition;

	Transform * transform = this->GetOwner()->GetComponent<Transform>();
	transform->SetPosition(NextPosition);
}
