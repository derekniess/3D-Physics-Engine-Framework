#include "Transform.h"


void Transform::Deserialize(TextFileData aTextData)
{
	// Get reference to data
	char * transformTextData = aTextData.pData;
	int  transformTextSize = aTextData.Size;
	// Counters
	int counterText = 0;
	int counterType = 0;
	int counterValue = 0;
	// Buffers
	char dataType[32];
	char dataValue[32];

	// Until end of data, populate the lists with appropriate values
	while (transformTextData[counterText] != '~')
	{
		// Zero out the buffers and counters after reading each set of data
		memset(dataType, '\0', 32 * sizeof(char));
		counterType = 0;
		memset(dataValue, '\0', 32 * sizeof(char));
		counterValue = 0;

		while (transformTextData[counterText] != '\n')    // Reads character by character until new line
		{
			dataType[counterType++] = transformTextData[counterText++];
		}

		if (strcmp(dataType, "Position") == 0)
		{
			counterText += 1;	// To skip the new line
			// Loop until reach the end of each line
			while (transformTextData[counterText] != '\n')
			{
				glm::vec3 newPosition;
				for (int i = 0; i < 3; ++i)
				{
					float value = 0.0f;
					while (transformTextData[counterText] != ',') // Read until a comma, one position value
					{
						dataValue[counterValue++] = transformTextData[counterText++];
					}
					counterText += 1; // Skip the comma
					value = strtof(dataValue, nullptr); // Convert to integral value
					newPosition[i] = value;
					// Reset the data value buffer
					memset(dataValue, '\0', 32 * sizeof(char));
					counterValue = 0;
				}
				// Set position value
				Position = newPosition;
			}
			counterText += 1;
		}
		else if (strcmp(dataType, "Rotation") == 0)
		{
			counterText += 1;	// To skip the new line
			// Loop until reach the end of each line
			while (transformTextData[counterText] != '\n')
			{
				glm::vec3 newRotation;
				for (int i = 0; i < 3; ++i)
				{
					float value = 0.0f;
					while (transformTextData[counterText] != ',') // Read until a comma, one rotation value
					{
						dataValue[counterValue++] = transformTextData[counterText++];
					}
					counterText += 1; // Skip the comma
					value = strtof(dataValue, nullptr); // Convert to integral value
					newRotation[i] = value;
					// Reset the data value buffer
					memset(dataValue, '\0', 32 * sizeof(char));
					counterValue = 0;
				}
				// Set rotation value
				Rotation = glm::quat(newRotation);
			}
			counterText += 1;
		}
		else if (strcmp(dataType, "Scale") == 0)
		{
			counterText += 1;	// To skip the new line
			// Loop until reach the end of each line
			while (transformTextData[counterText] != '\n')
			{
				glm::vec3 newScale;
				for (int i = 0; i < 3; ++i)
				{
					float value = 0.0f;
					while (transformTextData[counterText] != ',') // Read until a comma, one scale value
					{
						dataValue[counterValue++] = transformTextData[counterText++];
					}
					counterText += 1; // Skip the comma
					value = strtof(dataValue, nullptr); // Convert to integral value
					newScale[i] = value;
					// Reset the data value buffer
					memset(dataValue, '\0', 32 * sizeof(char));
					counterValue = 0;
				}
				// Set scale value
				Scale = newScale;
			}
			counterText += 1;
		}
	}
}

void Transform::Update()
{
	Rotation = glm::normalize(Rotation);
}

