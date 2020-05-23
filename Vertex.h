#pragma once
#include "glm/vec4.hpp"
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"

// Vertex layout used for regular meshes
struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec4 Color;
	glm::vec2 UVs;

	Vertex(glm::vec3 aPosition, glm::vec3 aNormal, glm::vec4 aColor, glm::vec2 aUVs) : 
		Position(aPosition),
		Normal(aNormal),
		Color(aColor),
		UVs(aUVs)
	{}
	Vertex() : 
	Position(glm::vec3(0)),
	Normal(glm::vec3(0)),
	Color(glm::vec4(1)),
	UVs(glm::vec2(0))
	{}
};