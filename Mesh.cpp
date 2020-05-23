#pragma once

#include "Mesh.h"
#include "ResourceManager.h"

Mesh::Mesh() : Primitive(Primitive::MESH)
{
	
}

Component::ComponentType Mesh::GetComponentID()
{
	return (ComponentType::PRIMITIVE);
}

void Mesh::Deserialize(TextFileData aTextData)
{
	// Get reference to data
	char * meshTextData = aTextData.pData;
	int meshTextSize = aTextData.Size;
	// Mesh data lists
	std::vector<glm::vec3> vertexPositionList;
	std::vector<glm::vec3> vertexColorList;
	std::vector<glm::vec2> vertexUVList;
	// Counters
	int counterText = 0;
	int counterType = 0;
	int counterValue = 0;
	// Buffers
	char dataType[32];
	char dataValue[32];
	
	// Until end of data, populate the lists with appropriate values
	while (meshTextData[counterText] != '~')
	{
		// Zero out the buffers and counters after reading each set of data
		memset(dataType, '\0', 32 * sizeof(char));
		counterType = 0;
		memset(dataValue, '\0', 32 * sizeof(char));
		counterValue = 0;

		while (meshTextData[counterText] != '\n')    // Reads character by character until new line
		{
			dataType[counterType++] = meshTextData[counterText++];
		}

		if (strcmp(dataType, "Vertices") == 0)
		{
			counterText += 3;	// To skip the newline character and the '{'
			// Loop until reach the end of vertex positions data
			while (meshTextData[counterText] != '}')
			{
				glm::vec3 newPosition;
				for (int i = 0; i < 3; ++i)
				{
					float value = 0.0f;
					while (meshTextData[counterText] != ',') // Read until a comma, one position value
					{
						dataValue[counterValue++] = meshTextData[counterText++];
					}
					counterText += 1; // Skip the comma
					value = strtof(dataValue, nullptr); // Convert to integral value
					newPosition[i] = value;
					// Reset the data value buffer
					memset(dataValue, '\0', 32 * sizeof(char));
					counterValue = 0;
				}
				// Add new position value to the list
				vertexPositionList.push_back(newPosition);
				counterText += 1; // Skip the new line
			}
			counterText += 2; // Skip the comma and new line
		}
		else if (strcmp(dataType, "Colors") == 0)
		{
			counterText += 3;	// To skip the newline character and the '{'
			// Loop until reach the end of vertex color data
			while (meshTextData[counterText] != '}')
			{
				glm::vec3 newColor;
				for (int i = 0; i < 3; ++i)
				{
					float value = 0.0f;
					while (meshTextData[counterText] != ',') // Read until a comma, one color value
					{
						dataValue[counterValue++] = meshTextData[counterText++];
					}
					counterText += 1; // Skip the comma
					value = strtol(dataValue, nullptr, 10); // Convert to integral value
					newColor[i] = value;

					// Reset the data value buffer
					memset(dataValue, '\0', 32 * sizeof(char));
					counterValue = 0;
				}
				// Add new color value to the list
				vertexColorList.push_back(newColor);
				counterText += 1; // Skip the new line
			}
			counterText += 2; // Skip the comma and new line
		}
		else if (strcmp(dataType, "UVs") == 0)
		{
			counterText += 3;	// To skip the newline character and the '{'
			// Loop until reach the end of vertex UV data
			while (meshTextData[counterText] != '}')
			{
				glm::vec2 newUV;
				for (int i = 0; i < 2; ++i)
				{
					float value = 0.0f;
					while (meshTextData[counterText] != ',') // Read until a comma, one UV value
					{
						dataValue[counterValue++] = meshTextData[counterText++];
					}
					counterText += 1; // Skip the comma
					value = strtol(dataValue, nullptr, 10); // Convert to integral value
					newUV[i] = value;

					// Reset the data value buffer
					memset(dataValue, '\0', 32 * sizeof(char));
					counterValue = 0;
				}
				// Add new color value to the list
				vertexUVList.push_back(newUV);
				counterText += 1; // Skip the new line
			}
			counterText += 2; // Skip the comma and new line
		}
		/*if (strcmp(DataType.c_str(), "Indices") == 0) TODO : ADD SUPPORT FOR INDEX SERIALIZATION
		{
		
		}*/
	}

	
	Vertices.reserve(vertexPositionList.size());

	for (int i = 0; i < vertexPositionList.size(); ++i)
	{
		Vertex newVertex;
		newVertex.Position = vertexPositionList[i];
		newVertex.Normal = glm::vec3(0, 0, 0);
		newVertex.Color = glm::vec4(vertexColorList[i], 1);
		newVertex.UVs = vertexUVList[i];
		Vertices.push_back(newVertex);
	}

	// Bind vertex data for this mesh with the newly obtained vertex list
	BindVertexData(Vertices);
}
