#pragma once
#include "glm\vec3.hpp"
#include "glm\mat3x3.hpp"
#include "InputManager.h"
#include "FrameRateController.h"

enum CameraType
{
	SPHERICAL,
	FREE,
	CameraTypeEnd
};

class Camera : public Observer
{
private:
	// View matrix variables
	glm::vec3 CameraPosition;
	glm::vec3 CameraLookDirection;
	glm::vec3 CameraUpDirection;

	float CameraPitch;
	float CameraYaw;
	float Sensitivity = 0.01f;
	
	// By default use free camera type
	CameraType Type = CameraType::FREE;

	InputManager & InputManagerReference;
	FramerateController & FrameControllerReference;
	
public:
	Camera(InputManager & input, FramerateController & frame) : InputManagerReference(input), FrameControllerReference(frame)
	{
		// I'm pretty sure the origin is always the origin
		CameraPosition = glm::vec3(0, 0, 10);
		// Negative Z axis is generally the look direction
		CameraLookDirection = glm::vec3(0, 0, -1);
		// Positive Y axis is generally the up direction
		CameraUpDirection = glm::vec3(0, 1, 0);

		CameraPitch = CameraYaw = 0.0f;
	}
	~Camera() {}

	inline glm::vec3 GetCameraPosition() { return CameraPosition; }
	inline glm::vec3 GetCameraLookDirection() { return CameraLookDirection; }
	inline glm::vec3 GetCameraUpDirection() { return CameraUpDirection; }
	inline float GetSensitivity() { return Sensitivity; }

	inline void SetCameraPosition(glm::vec3 aCameraPos) { CameraPosition = aCameraPos; }
	inline void SetCameraLookDirection(glm::vec3 aCameraLook) { CameraLookDirection = aCameraLook; }
	inline void SetCameraUpDirection(glm::vec3 aCameraUp) { CameraUpDirection = aCameraUp; }

	void Update();

private:
	virtual void OnNotify(Event * aEvent) override;
};