#pragma once
#include "Collider.h"
class Box : public Collider
{
	/*-----------MEMBER VARIABLES-----------*/
public:
	float Side = 2.0f;
	/*-----------MEMBER FUNCTIONS-----------*/
public:
	virtual void Initialize() override;

	virtual glm::vec3 FindFarthestPointInDirection(glm::vec3 aDirection);
};