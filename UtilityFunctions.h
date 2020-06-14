#pragma once
#include <glm/glm.hpp>
#include <list>
#include "Collider.h"
#include "Vertex.h"
#include "PhysicsUtilities.h"

namespace Utility
{
	
	// Sorts the input vector using the values provided by a reference vector of pairs, 
	// where the first element of the pair contains the order of rearrangement
	template <typename T>
	std::vector<T> SortUsingOrderingPairs(
		std::vector<T> const& aInputVector,
		std::vector<std::pair<int, float>> const& aReferenceVector
	) {
		std::vector<T> ret(aInputVector.size());

		size_t const size = aInputVector.size();
		for (size_t i = 0; i < size; ++i)
			ret[i] = aInputVector[aReferenceVector[i].first];

		return ret;
	}

	// Sorts a vector of pairs of <int, FloatIterator> based on the second value of the pairs
	// Assumption is that 1st item in pair is the index to be sorted, and 2nd item is value that decides sorting arrangement
	struct FloatOrdering {
		bool operator ()(std::pair<int, float> const& a, std::pair<int, float> const& b) {
			return a.second < b.second;
		}
	};

	// aDirection doesn't need to be normalized
	SupportPoint Support(Collider * aShape1, Collider * aShape2, glm::vec3 aDirection, glm:: mat4 & aModel1, glm::mat4 & aModel2);

	// Note that the following triple product expansion is used :
	// (A x B) x C = B(C.dot(A)) – A(C.dot(B)) to evaluate the triple product.
	inline glm::vec3 TripleCrossProduct(glm::vec3 A, glm::vec3 B, glm::vec3 C) {
		return (B * glm::dot(C, A)) - (A * glm::dot(C, B));
	}
	

	// Process the specified edge, if another edge with the same points in the
	// opposite order exists then it is removed and the new point is also not added
	// "The trick is that if two triangles that are removed share an edge then that edge is gone for good, 
	// but if an edge is used by only one removed triangle then it will form part of the hole."

	inline  void AddEdge(std::list<PolytopeEdge> & aEdgeList, const SupportPoint & a, const SupportPoint & b)
	{
		for (auto iterator = aEdgeList.begin(); iterator != aEdgeList.end(); ++iterator)
		{
			if (iterator->Points[0] == b && iterator->Points[1] == a)
			{
				// Encountered the same edge with opposite winding, remove it and don't add a new one
				aEdgeList.erase(iterator);
				return;
			}
		}
		aEdgeList.emplace_back(a, b);
	}

	// Code from Christer Ericson's Real-Time Collision Detection
	// Compute barycentric coordinates (u, v, w) for
	// point p with respect to triangle (a, b, c)
	inline void BarycentricProjection(glm::vec3 & aPoint, glm::vec3 & a, glm::vec3 & b, glm::vec3 & c, float & u, float & v, float & w)
	{
		glm::vec3 v0 = b - a, v1 = c - a, v2 = aPoint - a;
		float d00 = glm::dot(v0, v0);
		float d01 = glm::dot(v0, v1);
		float d11 = glm::dot(v1, v1);
		float d20 = glm::dot(v2, v0);
		float d21 = glm::dot(v2, v1);
		float denom = d00 * d11 - d01 * d01;
		v = (d11 * d20 - d01 * d21) / denom;
		w = (d00 * d21 - d01 * d20) / denom;
		u = 1.0f - v - w;
	}

	void CalculateMinkowskiDifference(std::vector<Vertex> & aMinkowskiDifference, Mesh * aShape1, Mesh * aShape2);

	// Optimized vector rotation by quaternion
	// https://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
	inline glm::vec3 RotateVectorByQuaternion(glm::vec3 aVector, glm::quat aQuat)
	{
		// Extract the vector part of the quaternion
		glm::vec3 u(aQuat.x, aQuat.y, aQuat.z);
		// Extract the scalar part of the quaternion
		float s = aQuat.w;
		// Do the math
		return ((2.0f * glm::dot(u, aVector) * u) 
			   + (s * s - glm::dot(u, u)) * aVector
			   + 2.0f * s * glm::cross(u, aVector));
	}
}