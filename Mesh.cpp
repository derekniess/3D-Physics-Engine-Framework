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
	std::vector<vector3> vertexPositionList;
	std::vector<vector3> vertexColorList;
	std::vector<vector2> vertexUVList;
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

		
	}

	
	Vertices.reserve(vertexPositionList.size());

	for (int i = 0; i < vertexPositionList.size(); ++i)
	{
		Vertex newVertex;
		newVertex.Position = vertexPositionList[i];
		newVertex.Normal = vector3(0, 0, 0);
		newVertex.Color = vector4(vertexColorList[i], 1);
		newVertex.UVs = vertexUVList[i];
		Vertices.push_back(newVertex);
	}

	// Bind vertex data for this mesh with the newly obtained vertex list
	BindVertexData(Vertices);
}
