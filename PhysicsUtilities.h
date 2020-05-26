#pragma once
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <algorithm>

struct SupportPoint
{
	glm::vec3 MinkowskiHullVertex;
	// World space support points
	glm::vec3 World_SupportPointA;
	glm::vec3 World_SupportPointB;
	// Object space support points
	glm::vec3 Local_SupportPointA;
	glm::vec3 Local_SupportPointB;

	bool operator == (const SupportPoint & ref) { return MinkowskiHullVertex == ref.MinkowskiHullVertex; }
};

// Used by the GJK Collision Handler. Changes from a point to a line, to a triangle to a tetrahedron
struct Simplex
{
	SupportPoint Vertices[4];
	int Size = 0;

	SupportPoint &a;
	SupportPoint &b;
	SupportPoint &c;
	SupportPoint &d;

	inline void Clear() { Size = 0; }
	Simplex() :
		a(Vertices[0]),
		b(Vertices[1]),
		c(Vertices[2]),
		d(Vertices[3])
	{}

	inline void Set(SupportPoint a, SupportPoint b, SupportPoint c, SupportPoint d) { Size = 4, this->a = a; this->b = b; this->c = c; this->d = d; }
	inline void Set(SupportPoint a, SupportPoint b, SupportPoint c) { Size = 3, this->a = a; this->b = b; this->c = c; }
	inline void Set(SupportPoint a, SupportPoint b) { Size = 2, this->a = a; this->b = b; }
	inline void Set(SupportPoint a) { Size = 1, this->a = a; }

	// Most recently added point is always added to the start of the list.
	// With every insertion all existing entries are shifted one to the right
	inline void Push(SupportPoint aNewVertex)
	{ 
		Size = std::min(Size + 1, 4);  
		
		for (int i = Size - 1; i > 0; i--)
			Vertices[i] = Vertices[i - 1];

		Vertices[0] = aNewVertex;
	}
};

struct PolytopeFace
{
	SupportPoint Points[3];
	glm::vec3 FaceNormal;

	PolytopeFace(const SupportPoint & supportA, const SupportPoint & supportB, const SupportPoint & supportC)
	{
		Points[0] = supportA;
		Points[1] = supportB;
		Points[2] = supportC;
		glm::vec3 edge1 = supportB.MinkowskiHullVertex - supportA.MinkowskiHullVertex;
		glm::vec3 edge2 = supportC.MinkowskiHullVertex - supportA.MinkowskiHullVertex;
		FaceNormal = glm::cross(edge1, edge2);
	}
};

struct PolytopeEdge
{
	SupportPoint Points[2];

	PolytopeEdge(const SupportPoint & supportA, const SupportPoint & supportB)
	{
		Points[0] = supportA;
		Points[1] = supportB;
	}
};

struct ContactData
{
	// Contact point data - World Space
	glm::vec3 ContactPositionA_WS;
	glm::vec3 ContactPositionB_WS;
	// Contact point data - Local Space
	glm::vec3 ContactPositionA_LS;
	glm::vec3 ContactPositionB_LS;

	// These 3 vectors form an orthonormal basis
	glm::vec3 Normal; // From collider A to collider B
	glm::vec3 Tangent1, Tangent2;

	float PenetrationDepth;

	glm::mat4 LocalToWorldMatrixA;
	glm::mat4 LocalToWorldMatrixB;
};

