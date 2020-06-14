#pragma once
#include "Typedefs.h"

// Vertex layout used for debug meshes - Only has position and color info.
struct DebugVertex
{
public:
	vector3 Position;
	vector4 Color;

	DebugVertex(vector3 aPosition, vector4 aColor) :
		Position(aPosition),
		Color(aColor)
	{}
	DebugVertex() :
		Position(vector3(0)),
		Color(vector4(1))
	{}
};