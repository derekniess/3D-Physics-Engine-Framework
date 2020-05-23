#pragma once
#include <GL/glew.h>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include "DebugVertex.h"

struct Arrow
{
	glm::vec3 PointA = glm::vec3(0);
	glm::vec3 PointB = glm::vec3(0);
	float Scale = 1.0f;
	static int VertexCount;
	glm::vec3 Color = glm::vec3(1, 0, 0);
	GLuint VAO;
	GLuint VBO;
	
	Arrow(glm::vec3 aPointA, glm::vec3 aPointB)
		: 
		PointA(aPointA),
		PointB(aPointB)
	{
		
	}
	~Arrow();
	inline void BindVertexData()
	{
		std::vector<DebugVertex> arrowVertices;

		// Arrow bar triangle 1
		arrowVertices.push_back(DebugVertex(glm::vec3(0.f, -0.1f, 0.f), glm::vec4(Color, 1)));
		arrowVertices.push_back(DebugVertex(glm::vec3(0.75f, -0.1f, 0.f), glm::vec4(Color, 1)));
		arrowVertices.push_back(DebugVertex(glm::vec3(0.f, 0.1f, 0.f), glm::vec4(Color, 1)));
		// Arrow bar triangle 2
		arrowVertices.push_back(DebugVertex(glm::vec3(0.75f, -0.1f, 0.f), glm::vec4(Color, 1)));
		arrowVertices.push_back(DebugVertex(glm::vec3(0.75f, 0.1f, 0.f), glm::vec4(Color, 1)));
		arrowVertices.push_back(DebugVertex(glm::vec3(0.f, 0.1f, 0.f), glm::vec4(Color, 1)));
		// Arrow tip triangle
		arrowVertices.push_back(DebugVertex(glm::vec3(0.75f, -0.2f, 0.f), glm::vec4(Color, 1)));
		arrowVertices.push_back(DebugVertex(glm::vec3(1.f, 0.f, 0.f), glm::vec4(Color, 1)));
		arrowVertices.push_back(DebugVertex(glm::vec3(0.75f, 0.2f, 0.f), glm::vec4(Color, 1)));

		GLsizei stride = sizeof(DebugVertex);
		Scale = glm::distance(PointA, PointB);
		int primitiveSize = stride * (int)arrowVertices.size();
		// Save vertex count for use later
		Arrow::VertexCount = (int)arrowVertices.size();
		/*--------------------------- VERTEX ARRAY OBJECT --------------------------------*/
		glBindVertexArray(VAO);
		/*--------------------------- VERTEX BUFFER OBJECT --------------------------------*/
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, primitiveSize, &arrowVertices[0], GL_STATIC_DRAW);
		/*--------------------------- VERTEX ATTRIBUTE POINTERS --------------------------------*/
		// Position
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			stride,
			(GLvoid*)0
		);
		glEnableVertexAttribArray(0);

		// Color
		glVertexAttribPointer(
			1,
			4,
			GL_FLOAT,
			GL_FALSE,
			stride,
			(GLvoid*)(3 * sizeof(GLfloat))
		);
		glEnableVertexAttribArray(1);

		// Unbind VAO
		glBindVertexArray(0);
	}
};