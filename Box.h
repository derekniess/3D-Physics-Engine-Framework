#pragma once
#include "Collider.h"
class Box : public Collider
{
	/*-----------MEMBER VARIABLES-----------*/
public:
	float Side = 2.0f;
	vector3 HalfSize;
	/*-----------MEMBER FUNCTIONS-----------*/
public:
	virtual void Initialize() override;
	virtual void Deserialize(TextFileData & aTextData) override {};
	virtual void Serialize(TextFileData & aTextData) override {};

	virtual vector3 FindFarthestPointInDirection(glm::vec3 aDirection);
};