#pragma once
#include "glm/vec4.hpp"
#include "glm/vec3.hpp"

// Vertex layout used for debug meshes - Only has position and color info.
struct DebugVertex
{
public:
	glm::vec3 Position;
	glm::vec4 Color;

	DebugVertex(glm::vec3 aPosition, glm::vec4 aColor) :
		Position(aPosition),
		Color(aColor)
	{}
	DebugVertex() :
		Position(glm::vec3(0)),
		Color(glm::vec4(1))
	{}
};