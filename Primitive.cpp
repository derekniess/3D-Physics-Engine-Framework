#include "Primitive.h"


void Primitive::SetVertexColorsUniform(glm::vec3 aNewColor)
{
	for (int i = 0; i < Vertices.size(); ++i)
	{
		Vertices[i].Color = glm::vec4(aNewColor, 1);
	}
	// Rebuffer the VBO once colors have been changed
	BindVertexData(Vertices);
}

void Primitive::BindVertexData(std::vector<Vertex>& aVertexData)
{
	int primitiveSize = sizeof(Vertex) * (int)aVertexData.size();
	PrimitiveSize = primitiveSize;
	/*--------------------------- VERTEX ARRAY OBJECT --------------------------------*/
	glBindVertexArray(VAO);
	/*--------------------------- VERTEX BUFFER OBJECT --------------------------------*/
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, primitiveSize, &aVertexData[0], GL_STATIC_DRAW);
	/*--------------------------- VERTEX ATTRIBUTE POINTERS --------------------------------*/
	// Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(GLvoid*)0
	);

	// Normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(GLvoid*)(3 * sizeof(GLfloat))
	);
	
	// Color
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,
		4,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(GLvoid*)(6 * sizeof(GLfloat))
	);
	
	// Texture Coordinates
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(
		3,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(GLvoid*)(10 * sizeof(GLfloat))
	);
	bIsBound = true;
	// Unbind VAO
	glBindVertexArray(0);
}

void Primitive::ApplyTexture(unsigned int aTextureID)
{
	PrimitiveEvent TextureEvent;
	TextureEvent.EventID = PrimitiveEvent::TEXTURE_REQUEST;
	TextureEvent.TextureID = aTextureID;
	TextureRequest.NotifyAllObservers(&TextureEvent);
}

void Primitive::Update()
{
}

void Primitive::Debuffer()
{
	glBindVertexArray(VAO);
	/*--------------------------- VERTEX BUFFER OBJECT --------------------------------*/
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW);

	glBindVertexArray(0);
	bIsBound = false;
}
