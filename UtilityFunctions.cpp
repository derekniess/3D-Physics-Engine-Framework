#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "UtilityFunctions.h"
#include "Transform.h"
#include "GameObject.h"
#include "Mesh.h"

// aDirection doesn't need to be normalized
SupportPoint Utility::Support(Collider * aShape1, Collider * aShape2, vector3 aDirection, matrix4 & aModel1, matrix4 & aModel2)
{
	SupportPoint newSupportPoint;
	aDirection = glm::normalize(aDirection);

	// Get points on edge of the shapes in opposite directions, in object space
	newSupportPoint.Local_SupportPointA = aShape1->FindFarthestPointInDirection(aDirection);
	newSupportPoint.Local_SupportPointB = aShape2->FindFarthestPointInDirection(-aDirection);
	
	// Convert to world space
	newSupportPoint.World_SupportPointA = vector3(aModel1 * glm::vec4(newSupportPoint.Local_SupportPointA, 1));
	newSupportPoint.World_SupportPointB = vector3(aModel2 * glm::vec4(newSupportPoint.Local_SupportPointB, 1));

	// Perform the Minkowski Difference
	newSupportPoint.MinkowskiHullVertex = newSupportPoint.World_SupportPointA - newSupportPoint.World_SupportPointB;
	return newSupportPoint;
}

void Utility::CalculateMinkowskiDifference(std::vector<Vertex>& aMinkowskiDifference, Mesh * aShape1, Mesh * aShape2)
{
	int size1 = (int)aShape1->Vertices.size();
	int size2 = (int)aShape2->Vertices.size();
	std::vector<Vertex> MinkowskiDifferenceVertices;

	for (int i = 0; i < size1; ++i)
	{
		// Calculate the model matrices and set the matrix uniform
		matrix4 model1, model2;
		matrix4 translate, rotate, scale;
		Transform * transform1 = aShape1->GetOwner()->GetComponent<Transform>();
		translate = glm::translate(transform1->GetPosition());
		rotate = glm::mat4_cast(transform1->GetRotation());
		scale = glm::scale(transform1->GetScale());
		model1 = translate * rotate * scale;

		Transform * transform2 = aShape2->GetOwner()->GetComponent<Transform>();

		translate = glm::translate(transform2->GetPosition());
		rotate = glm::mat4_cast(transform2->GetRotation());
		scale = glm::scale(transform2->GetScale());
		model2 = translate * rotate * scale;

		vector4 position1 = model1 * vector4(aShape1->Vertices[i].Position, 1);
		Vertex newVertex;
		for (int j = 0; j < size2; ++j)
		{
			vector4 position2 = model2 * vector4(aShape2->Vertices[j].Position, 1);
			newVertex.Position = vector3(position2 - position1);
			MinkowskiDifferenceVertices.push_back(newVertex);
		}
	}

	// Sort Minkowski Difference vertices
	// https://stackoverflow.com/questions/6880899/sort-a-set-of-3-d-points-in-clockwise-counter-clockwise-order
	vector3 zAxis = vector3(0, 0, 1);
	vector3 xAxis = vector3(1, 0, 0);

	std::vector<std::pair<int, float>> order(MinkowskiDifferenceVertices.size());

	for (int i = 0; i < MinkowskiDifferenceVertices.size(); ++i)
	{
		float zAngle = glm::dot(MinkowskiDifferenceVertices[i].Position, zAxis);
		float xAngle = glm::dot(MinkowskiDifferenceVertices[i].Position, xAxis);
		float angle = atan2(zAngle, xAngle);
		order[i] = std::make_pair(i, angle);
	}

	std::sort(order.begin(), order.end(), Utility::FloatOrdering());
	Utility::SortUsingOrderingPairs(MinkowskiDifferenceVertices, order);
	
	aMinkowskiDifference = std::move(MinkowskiDifferenceVertices);

}
