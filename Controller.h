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

	glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 leftVector = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 forwardVector = glm::vec3(0.0f, 0.0f, -1.0f);
public:
	float MovementSpeed = 1.f;
	Transform * TargetTransform = nullptr;

	/*----------MEMBER FUNCTIONS----------*/
public:
	Controller(InputManager const & in, FramerateController const & frame) : InputManagerReference(in), FrameRateControllerReference(frame), Component(Component::CONTROLLER) {}
	virtual ~Controller() {};

	static inline ComponentType GetComponentID() { return Component::ComponentType::CONTROLLER; }
	static inline const char * GetComponentName() { return ComponentTypeName[ComponentType::CONTROLLER]; }

	virtual void Deserialize(TextFileData aTextData) override {};
	virtual void Update() override;
};