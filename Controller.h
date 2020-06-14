#pragma once
#include "Component.h"
#include "Transform.h"
#include "InputManager.h"
#include "FrameRateController.h"

class Controller : public Component
{
	/*----------MEMBER VARIABLES----------*/
private:
	InputManager const & InputManagerReference;
	FramerateController const & FrameRateControllerReference;

	vector3 upVector = vector3(0.0f, 1.0f, 0.0f);
	vector3 leftVector = vector3(1.0f, 0.0f, 0.0f);
	vector3 forwardVector = vector3(0.0f, 0.0f, -1.0f);
public:
	float MovementSpeed = 1.f;
	Transform * TargetTransform = nullptr;

	/*----------MEMBER FUNCTIONS----------*/
public:
	Controller(InputManager const & in, FramerateController const & frame) : InputManagerReference(in), FrameRateControllerReference(frame), Component(Component::CONTROLLER) {}
	virtual ~Controller() {};

	static inline ComponentType GetComponentID() { return Component::ComponentType::CONTROLLER; }
	static inline const char * GetComponentName() { return ComponentTypeName[ComponentType::CONTROLLER]; }
	virtual void Update() override;

	virtual void Deserialize(TextFileData & aTextData) override {};
	virtual void Serialize(TextFileData & aTextData) override {};
};