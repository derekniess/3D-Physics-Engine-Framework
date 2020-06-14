#pragma once
#include <algorithm>
#include "Typedefs.h"

struct SupportPoint
{
	vector3 MinkowskiHullVertex;
	// World space support points
	vector3 World_SupportPointA;
	vector3 World_SupportPointB;
	// Object space support points
	vector3 Local_SupportPointA;
	vector3 Local_SupportPointB;

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
		vector3 edge1 = supportB.MinkowskiHullVertex - supportA.MinkowskiHullVertex;
		vector3 edge2 = supportC.MinkowskiHullVertex - supportA.MinkowskiHullVertex;
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
	vector3 ContactPositionA_WS;
	vector3 ContactPositionB_WS;
	// Contact point data - Local Space
	vector3 ContactPositionA_LS;
	vector3 ContactPositionB_LS;

	// These 3 vectors form an orthonormal basis
	vector3 Normal; // From collider A to collider B
	vector3 Tangent1, Tangent2;

	float PenetrationDepth;
};


struct ContactManifold
{
	// Slot of this manifold in the manifold objects list
	int ManifoldSlot = 0;
	// Slot of contact constraint associated with this manifold
	int ConstraintID = 0; 
	ContactData ManifoldPoints[4];
	int Size = 0;

	ContactData &a;
	ContactData &b;
	ContactData &c;
	ContactData &d;

	void ValidateAllContacts();
	void ValidateNewContact();
	void EliminateExtraContacts();

	inline void Clear() { Size = 0; }
	ContactManifold() :
		a(ManifoldPoints[0]),
		b(ManifoldPoints[1]),
		c(ManifoldPoints[2]),
		d(ManifoldPoints[3])
	{}

	inline void Set(ContactData a, ContactData b, ContactData c, ContactData d) { Size = 4, this->a = a; this->b = b; this->c = c; this->d = d; }
	inline void Set(ContactData a, ContactData b, ContactData c) { Size = 3, this->a = a; this->b = b; this->c = c; }
	inline void Set(ContactData a, ContactData b) { Size = 2, this->a = a; this->b = b; }
	inline void Set(ContactData a) { Size = 1, this->a = a; }

	// Most recently added point is always added to the start of the list.
	// With every insertion all existing entries are shifted one to the right
	inline void Push(ContactData aNewContact)
	{
		Size = std::min(Size + 1, 4);

		for (int i = Size - 1; i > 0; i--)
			ManifoldPoints[i] = ManifoldPoints[i - 1];

		ManifoldPoints[0] = aNewContact;
	}
};

struct Point
{
	vector3 position;
	Point(vector3 p) : position(p)
	{}
};

struct LineSegment
{
	Point A;
	Point B;
	LineSegment(vector3 a, glm::vec3 b) : A(a), B(b)
	{}
};

// Closest point method taken from Erin Catto's GDC 2010 slides
// Returns the closest point
inline Point ClosestPointOnLineFromTargetPoint(LineSegment & aLine, Point & aTargetPoint, float & u, float & v)
{
	vector3 lineSegment = aLine.B.position - aLine.A.position;

	vector3 normalized = glm::normalize(lineSegment);
	v = glm::dot(-aLine.A.position, normalized) / glm::length(lineSegment);
	u = glm::dot(aLine.B.position, normalized) / glm::length(lineSegment);
	vector3 closestPoint;
	if (u <= 0.0f)
	{
		closestPoint = aLine.B.position;
	}
	else if (v <= 0.0f)
	{
		closestPoint = aLine.A.position;
	}
	else
	{
		closestPoint = u * aLine.A.position + v * aLine.B.position;
	}

	return Point(closestPoint);
}