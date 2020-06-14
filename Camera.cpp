#include "Camera.h"
#include "Engine.h"

void Camera::OnNotify(Event * aEvent)
{
	// Check if this is an Engine event
	EngineEvent * engineEvent = static_cast<EngineEvent *>(aEvent);
	if (engineEvent)
	{
		switch (engineEvent->EventID)
		{
			case EngineEvent::EventList::ENGINE_TICK:
			{
				Update();
			}
		}
	}
}

void Camera::Update()
{
	float cameraSpeed = 10.0f * FrameControllerReference.DeltaTime;
	vector3 cameraTranslation;
	switch (Type)
	{
		case CameraType::FREE:
		{
			if (InputManagerReference.isKeyPressed(GLFW_KEY_W))
			{
				cameraTranslation = CameraLookDirection * cameraSpeed;
				CameraPosition += cameraTranslation;
			}
			if (InputManagerReference.isKeyPressed(GLFW_KEY_A))
			{
				cameraTranslation = glm::cross(CameraLookDirection, CameraUpDirection);
				cameraTranslation = glm::normalize(cameraTranslation);
				cameraTranslation *= cameraSpeed;
				// Subtract to move left
				CameraPosition -= cameraTranslation;
			}
			if (InputManagerReference.isKeyPressed(GLFW_KEY_S))
			{
				// Subtract to move backwards
				cameraTranslation = CameraLookDirection * cameraSpeed;
				CameraPosition -= cameraTranslation;
			}
			if (InputManagerReference.isKeyPressed(GLFW_KEY_D))
			{
				cameraTranslation = glm::cross(CameraLookDirection, CameraUpDirection);
				cameraTranslation = glm::normalize(cameraTranslation);
				cameraTranslation *= cameraSpeed;
				// Add to move right
				CameraPosition += cameraTranslation;
			}
			if (InputManagerReference.isKeyPressed(GLFW_KEY_Q))
			{
				cameraTranslation = CameraUpDirection * cameraSpeed;
				CameraPosition += cameraTranslation;
			}
			if (InputManagerReference.isKeyPressed(GLFW_KEY_E))
			{
				cameraTranslation = CameraUpDirection * cameraSpeed;
				CameraPosition -= cameraTranslation;
			}
		}
			break;
		case CameraType::SPHERICAL:
			break;
	}
	
}
