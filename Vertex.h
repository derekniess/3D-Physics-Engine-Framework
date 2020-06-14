#pragma once
#include "Typedefs.h"

// Vertex layout used for regular meshes
struct Vertex
{
	vector3 Position;
	vector3 Normal;
	vector4 Color;
	vector2 UVs;

	Vertex(vector3 aPosition, vector3 aNormal, vector4 aColor, vector2 aUVs) : 
		Position(aPosition),
		Normal(aNormal),
		Color(aColor),
		UVs(aUVs)
	{}
	Vertex() : 
	Position(vector3(0)),
	Normal(vector3(0)),
	Color(vector4(1)),
	UVs(vector2(0))
	{}
};