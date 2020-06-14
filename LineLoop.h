#pragma once
#include "Typedefs.h"
#include <vector>

struct LineLoop
{
public:
	LineLoop() {}
	std::vector<DebugVertex> LineLoopVertices;
	vector4 Color = vector4(1, 0, 0, 1);
	GLuint VAO;
	GLuint VBO;
	// Index of slot this primitive occupies in renderer registry 
	int RegistryID;
	inline void AddVertex(vector3 &aPosition)
	{
		// Don't add degenerate values
		if (aPosition == vector3(0, 0, 0))
			return;
		DebugVertex newVertex(aPosition, Color);
		LineLoopVertices.push_back(newVertex);
	}

	inline void BindVertexData()
	{
		GLsizei stride = sizeof(DebugVertex);
		int primitiveSize = stride * (int)LineLoopVertices.size();
		/*--------------------------- VERTEX ARRAY OBJECT --------------------------------*/
		glBindVertexArray(VAO);
		/*--------------------------- VERTEX BUFFER OBJECT --------------------------------*/
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, primitiveSize, &LineLoopVertices[0], GL_STATIC_DRAW);
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