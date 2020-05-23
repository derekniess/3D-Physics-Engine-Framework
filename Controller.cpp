#include "Controller.h"
#include "Physics.h"
#include "GameObject.h"

void Controller::Update()
{
	if (InputManagerReference.isKeyPressed(GLFW_KEY_UP))
	{
		TargetTransform->Position += upVector * MovementSpeed * FrameRateControllerReference.DeltaTime;
	}
	if (InputManagerReference.isKeyPressed(GLFW_KEY_DOWN))
	{
		TargetTransform->Position += upVector * (-MovementSpeed) * FrameRateControllerReference.DeltaTime;
	}

	// Movement and rotation variants
	if (InputManagerReference.isKeyPressed(GLFW_KEY_LEFT) && InputManagerReference.isKeyPressed(GLFW_KEY_LEFT_CONTROL))
	{
		glm::quat yawRotation = glm::angleAxis(-0.1f, glm::vec3(0, 1, 0));
		TargetTransform->Rotation *= yawRotation;
	}
	else if(InputManagerReference.isKeyPressed(GLFW_KEY_LEFT) && InputManagerReference.isKeyPressed(GLFW_KEY_LEFT_ALT))
	{
		glm::quat pitchRotation = glm::angleAxis(-0.1f, glm::vec3(1, 0, 0));
		TargetTransform->Rotation *= pitchRotation;
	}
	else if (InputManagerReference.isKeyPressed(GLFW_KEY_LEFT) && InputManagerReference.isKeyPressed(GLFW_KEY_LEFT_SHIFT))
	{
		glm::quat rollRotation = glm::angleAxis(-0.1f, glm::vec3(0, 0, 1));
		TargetTransform->Rotation *= rollRotation;
	}
	else if (InputManagerReference.isKeyPressed(GLFW_KEY_LEFT))
	{
		TargetTransform->Position += leftVector * MovementSpeed * FrameRateControllerReference.DeltaTime;
	}
	
	// Movement and rotation variants
	if (InputManagerReference.isKeyPressed(GLFW_KEY_RIGHT) && InputManagerReference.isKeyPressed(GLFW_KEY_LEFT_CONTROL))
	{
		glm::quat yawRotation = glm::angleAxis(0.1f, glm::vec3(0, 1, 0));
		TargetTransform->Rotation *= yawRotation;
	}
	else if (InputManagerReference.isKeyPressed(GLFW_KEY_RIGHT) && InputManagerReference.isKeyPressed(GLFW_KEY_LEFT_ALT))
	{
		glm::quat pitchRotation = glm::angleAxis(0.1f, glm::vec3(1, 0, 0));
		TargetTransform->Rotation *= pitchRotation;
	}
	else if (InputManagerReference.isKeyPressed(GLFW_KEY_RIGHT) && InputManagerReference.isKeyPressed(GLFW_KEY_LEFT_SHIFT))
	{
		glm::quat rollRotation = glm::angleAxis(0.1f, glm::vec3(0, 0, 1));
		TargetTransform->Rotation *= rollRotation;
	}
	else if (InputManagerReference.isKeyPressed(GLFW_KEY_RIGHT))
	{
		TargetTransform->Position += leftVector * (-MovementSpeed) * FrameRateControllerReference.DeltaTime;
	}

	

	// If owner has a physics component, update it directly along with the transform
	Physics * physics = nullptr;
	physics = GetOwner()->GetComponent<Physics>();
	if (physics)
	{
		physics->SetCurrentPosition(TargetTransform->Position);
	}
}
