#pragma once
#include <GL/glew.h>
#include "Typedefs.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

struct Quad
{
	vector3 WorldPosition = vector3(0);
	float Scale = 1.0f;
	static const int VertexCount = 6;
	vector3 Color = vector3(0, 0, 1); 
	GLuint VAO;
	GLuint VBO;
	Quad(vector3 aPosition) :
		WorldPosition(aPosition)
	{} 

	inline void BindVertexData()
	{
		float QuadVertices[] =
		{
			-0.5f, 0.5f, 0.f, Color.x, Color.y, Color.z, 1.f,	// Top-left
			0.5f, 0.5f, 0.f, Color.x, Color.y, Color.z, 1.f,    // Top-right
			0.5f, -0.5f, 0.f, Color.x, Color.y, Color.z, 1.f,	// Bottom-right
			0.5f, -0.5f, 0.f, Color.x, Color.y, Color.z, 1.f,   // Bottom-right
			-0.5f, -0.5f, 0.f, Color.x, Color.y, Color.z, 1.f,	// Bottom-left
			-0.5f, 0.5f, 0.f, Color.x, Color.y, Color.z, 1.f,	// Top-left
		};														

		GLsizei stride = 7 * sizeof(GLfloat);
		int primitiveSize = stride * VertexCount;
		/*--------------------------- VERTEX ARRAY OBJECT --------------------------------*/
		glBindVertexArray(VAO);
		/*--------------------------- VERTEX BUFFER OBJECT --------------------------------*/
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, primitiveSize, &QuadVertices[0], GL_STATIC_DRAW);
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