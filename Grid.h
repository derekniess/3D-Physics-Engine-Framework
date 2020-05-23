#pragma once
#include <GL/glew.h>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include "DebugVertex.h"
#include "Vertex.h"

struct Grid
{
public:
	int SectionsX = 10;
	int SectionsY = 10;
	float SizeX = 1;
	float SizeY = 1;
	float SpacingX = 1.0f;
	float SpacingY = 1.0f;
	
	std::vector<DebugVertex> GridPoints;
	std::vector<Vertex> GridVertices;
	glm::vec3 Color = glm::vec3(1, 0, 0);
	GLuint VAO;
	GLuint VBO;

  /* Argument 1 - Sections X
	 Argument 2 - Sections Y
	 Argument 3 - Size X
	 Argument 4 - Size Y  */
	Grid(int aSectionsX, int aSectionsY, float aSizeX, float aSizeY)
		:
		SectionsX(aSectionsX),
		SectionsY(aSectionsY),
		SizeX(aSizeX),
		SizeY(aSizeY)
	{
		GridPoints.reserve(SectionsX * SectionsY);
		for (int i = 0; i < SectionsX; ++i)
		{
			for (int j = 0; j < SectionsY; ++j)
			{
				GridPoints.push_back(DebugVertex(glm::vec3(i * (SpacingX), 0, j * (SpacingY)), glm::vec4(1)));
			}
		}
		CalculateGrid();
	}
	~Grid() {}
	inline void CalculateGrid()
	{
		GridVertices.clear();

		for (int i = 0; i < SectionsX; ++i)
		{
			for (int j = 0; j < SectionsY; ++j)
			{
				int index = (i * SectionsY) + j;

				int k = index + index / (SectionsX - 1);
				int a = k;
				int b = k + 1;
				int c = k + 1 + SectionsX;
				int d = k + SectionsX;

				Vertex newGridVertex;
				newGridVertex.Color = GridPoints[index].Color;

				glm::vec3 vertexA;
				glm::vec3 vertexB;
				glm::vec3 vertexC;
				glm::vec3 vertexD;
				ComputePointCoordinates(a, vertexA);
				ComputePointCoordinates(b, vertexB);
				ComputePointCoordinates(c, vertexC);
				ComputePointCoordinates(d, vertexD);

				// Triangle 1
				// Add grid vertex to vertex array
				newGridVertex.Position = vertexA;
				GridVertices.push_back(newGridVertex);
				// Add grid vertex to vertex array
				newGridVertex.Position = vertexC;
				GridVertices.push_back(newGridVertex);
				// Add grid vertex to vertex array
				newGridVertex.Position = vertexD;
				GridVertices.push_back(newGridVertex);

				// Triangle 2
				// Add grid vertex to vertex array
				newGridVertex.Position =vertexA;
				GridVertices.push_back(newGridVertex);
				// Add grid vertex to vertex array
				newGridVertex.Position = vertexB;
				GridVertices.push_back(newGridVertex);
				// Add grid vertex to vertex array
				newGridVertex.Position = vertexC;
				GridVertices.push_back(newGridVertex);
			}
		}
	}
	inline void ComputePointCoordinates(int aIndex, glm::vec3 & aPoint)
	{
		glm::vec3 temp = ComputePointCoordinates(aIndex);
		aPoint = temp;
	}

	inline glm::vec3 ComputePointCoordinates(int aIndex)
	{
		glm::vec3 point(0);
		float width = SpacingX * (SectionsX - 1);
		float height = SpacingY * (SectionsY - 1);
		float minX = -width / 2;
		float minY = -height / 2;

		point.x = minX + SpacingX * (aIndex % SectionsX);
		if(aIndex < SectionsX * SectionsY)
			point.y = GridPoints[aIndex].Position.y;
		point.z = minY + SpacingY * (aIndex / SectionsX);

		return point;
	}
};