#pragma once
#include "Observer.h"
#include "GameObject.h"
#include "PhysicsUtilities.h"

class CollideEvent : public Event
{
public:
	GameObject * mpObject1;
	GameObject * mpObject2;

	CollideEvent();// : Event(COLLIDE) { mpObject1 = nullptr; mpObject2 = nullptr; }
	virtual ~CollideEvent() {}
};

class FramerateController;
class InputManager;
class Physics;
class Collider;
class Constraint;

class PhysicsManager : public Observer
{
	/*----------MEMBER VARIABLES----------*/
public:
	static int IntegratorIterations;
	const static int ConstraintSolverIterations = 4;
	// Stability analysis provides an upper bound of β ≤ 1/∆t for smooth decay
	float BaumgarteScalar = 0.1f;
	bool bShouldSimulate = true;
	/*---ENGINE REFERENCE ---*/
	Engine & EngineHandle;

	std::vector<Physics *> PhysicsObjectsList;
	std::vector<Collider *> ColliderObjectsList;
	std::vector<Constraint *> ConstraintObjectsList;
	/*----------MEMBER FUNCTIONS----------*/
	PhysicsManager(Engine & aEngine) :EngineHandle(aEngine) {};
	~PhysicsManager() {};
	
	// Registration functions
	void RegisterPhysicsObject(Physics * aNewPhysics);
	void RegisterColliderObject(Collider * aNewCollider);
	void RegisterConstraintObject(Constraint * aNewConstraint);

	// Main function of physics manager, calls all other functions
	void Update();

	// Performs integration of all physics objects
	void Simulation();

	// Detects collision between all pairs of collider objects
	void DetectCollision();
	bool GJKCollisionHandler(Collider * aCollider1, Collider * aCollider2, ContactData & aContactData);
	bool EPAContactDetection(Simplex & aSimplex, Collider * aShape1, Collider * aShape2, ContactData & aContactData);
	bool ExtrapolateContactInformation(PolytopeFace * aClosestFace, ContactData & aContactData);
	bool CheckIfSimplexContainsOrigin(Simplex & aSimplex, glm::vec3 & aSearchDirection);

	// Resolves pairwise constraints that are violated
	void SolveConstraints();

	virtual void OnNotify(Event * aEvent) override;

};