#pragma once
#include "Typedefs.h"
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
	vector3 CameraPosition;
	vector3 CameraLookDirection;
	vector3 CameraUpDirection;

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
		CameraPosition = vector3(0, 0, 10);
		// Negative Z axis is generally the look direction
		CameraLookDirection = vector3(0, 0, -1);
		// Positive Y axis is generally the up direction
		CameraUpDirection = vector3(0, 1, 0);

		CameraPitch = CameraYaw = 0.0f;
	}
	~Camera() {}

	inline vector3 GetCameraPosition() { return CameraPosition; }
	inline vector3 GetCameraLookDirection() { return CameraLookDirection; }
	inline vector3 GetCameraUpDirection() { return CameraUpDirection; }
	inline float GetSensitivity() { return Sensitivity; }

	inline void SetCameraPosition(vector3 aCameraPos) { CameraPosition = aCameraPos; }
	inline void SetCameraLookDirection(vector3 aCameraLook) { CameraLookDirection = aCameraLook; }
	inline void SetCameraUpDirection(vector3 aCameraUp) { CameraUpDirection = aCameraUp; }

	void Update();

private:
	virtual void OnNotify(Event * aEvent) override;
};