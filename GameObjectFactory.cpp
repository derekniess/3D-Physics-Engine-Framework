#include <cstdio>

#include "GameObjectFactory.h"
#include "ResourceManager.h"
#include "Mesh.h"
GameObject * GameObjectFactory::SpawnGameObjectFromArchetype(const char * aFileName)
{
	// Create game object with default transform
	GameObject * newGameObject = new GameObject(EngineHandle);
	// Create root component from supplied/default transform
	Transform * rootComponent = new Transform();
	rootComponent->SetOwner(newGameObject);
	// Add component pointer to newly created game object
	newGameObject->AddComponent(rootComponent);

	rootComponent->SetScale(glm::vec3(3));
	// Read archetype data from file
	TextFileData archetypeData;
	archetypeData = EngineHandle.GetResourceManager().LoadTextFile(aFileName, READ);

	char * archetypeContents = archetypeData.pData;
	char componentName[64];

	unsigned int counterText = 0;
	unsigned int counterComponentName = 0;
	
	while (archetypeContents[counterText] != '\0')		// While not end of file
	{
		// Zero out the component name array and counters
		counterComponentName = 0;
		memset(componentName, '\0', 64 * sizeof(char));
		while (archetypeContents[counterText] != '\n') // Reads character by character until new line
		{
			componentName[counterComponentName++] = archetypeContents[counterText++];
		}
		if (strcmp(componentName, "Mesh") == 0)	
		{
			// Find size of mesh text data
			int meshTextDataSize = counterText + 1;  // Skip new line character
			while (archetypeContents[meshTextDataSize] != '~') { meshTextDataSize++; } // Check for end of component data character
			meshTextDataSize -= counterText; // To get size of text data related to mesh alone

			// Allocate memory for text data and null it out
			char * meshTextData = new char[meshTextDataSize];
			memset(meshTextData, '\0', sizeof(char) * meshTextDataSize);

			// Set the text data from archetype contents
			for (int i = 0; i < meshTextDataSize; ++i)
			{
				meshTextData[i] = archetypeContents[counterText + 1 + i];
			}
			
			// Create component and serialize
			Mesh * meshComponent = SpawnComponent<Mesh>();
			meshComponent->SetOwner(newGameObject);
			TextFileData meshData;
			meshData.pData = meshTextData;
			meshData.Size = meshTextDataSize;
			meshComponent->Deserialize(meshData);
			newGameObject->AddComponent(meshComponent);
			counterText += meshTextDataSize - 1;
		}
		else if (strcmp(componentName, "Transform") == 0)
		{
			// Find size of transform text data
			int transformTextDataSize = counterText + 1;  // Skip new line character
			while (archetypeContents[transformTextDataSize] != '~') { transformTextDataSize++; } // Check for end of component data character
			transformTextDataSize -= counterText; // To get size of text data related to transform alone

			// Allocate memory for text data and null it out
			char * transformTextData = new char[transformTextDataSize];
			memset(transformTextData, '\0', sizeof(char) * transformTextDataSize);

			// Set the text data from archetype contents
			for (int i = 0; i < transformTextDataSize; ++i)
			{
				transformTextData[i] = archetypeContents[counterText + 1 + i];
			}

			// Create component and serialize
			Transform * transformComponent = SpawnComponent<Transform>();
			transformComponent->SetOwner(newGameObject);
			TextFileData transformData;
			transformData.pData = transformTextData;
			transformData.Size = transformTextDataSize;
			transformComponent->Deserialize(transformData);
			newGameObject->AddComponent(transformComponent);
		}
	
		counterText += 3; // To skip newline and carriage return characters
		if (counterText > archetypeData.Size)
			break;
	}
	
	// Add constructed game object to list
	GameObjectList.emplace_back(newGameObject);
	
	// Add game object to observer list of engine tick event
	EngineHandle.GetMainEventList()[EngineEvent::ENGINE_TICK].AddObserver(newGameObject);
	return newGameObject;

}

GameObject * GameObjectFactory::SpawnGameObject(Transform & aTransform)
{
	GameObject * newGameObject = new GameObject(EngineHandle);
	newGameObject->Name = std::string("Object") + std::to_string(GameObjectList.size() + 1);
	// Create root component from supplied/default transform
	Transform * rootComponent = new Transform(aTransform);
	rootComponent->SetOwner(newGameObject);
	// Add component pointer to newly created game object
	newGameObject->AddComponent(rootComponent);
	// Add game object pointer to list
	GameObjectList.emplace_back(newGameObject);

	// Add game object to observer list of engine load event
	EngineHandle.GetMainEventList()[EngineEvent::ENGINE_LOAD].AddObserver(newGameObject);
	// Add game object to observer list of engine tick event
	EngineHandle.GetMainEventList()[EngineEvent::ENGINE_TICK].AddObserver(newGameObject);
	// Add game object to observer list of engine exit event
	EngineHandle.GetMainEventList()[EngineEvent::ENGINE_EXIT].AddObserver(newGameObject);

	return newGameObject;
}

void GameObjectFactory::OnNotify(Event * aEvent)
{
	// Check if this is an Engine event
	EngineEvent * engineEvent = static_cast<EngineEvent *>(aEvent);
	if (engineEvent)
	{
		switch (engineEvent->EventID)
		{
			case EngineEvent::EventList::ENGINE_INIT:
			{
				
			}
		}
	}
}
