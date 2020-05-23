#pragma once

#include <string>
#include <utility>
#include <vector>
#include <memory>

#include "Observer.h"
#include "Texture.h"
#include "Resource.h"
#include "DebugVertex.h"

class Renderer;
class Mesh;

enum AccessType
{
	READ,
	WRITE,
	APPEND,
	AccessTypeEnd
};

struct TextFileData
{
	char * pData;
	unsigned int Size;
	TextFileData() : 
		pData(nullptr), 
		Size(0) 
	{}
};
class Engine;

class ResourceManager : public Observer
{
private:
	std::vector<std::unique_ptr<Texture>> TextureList;
	/*------------------------------- ENGINE REFERENCE -------------------------------*/
	Engine & EngineHandle;

public:
	ResourceManager(Engine & aEngine) :EngineHandle(aEngine) {};
	virtual ~ResourceManager() {};

	inline Texture * GetTexture(int aTextureID) const { return TextureList[aTextureID].get(); }

	TextFileData & LoadTextFile(const char* aFileName, AccessType aAccessType) const;
	
	Texture * LoadTexture(int aWidth, int aHeight, char * aFilename);
	
	// Uses Assimp importer to read mesh data from file and returns it in a Mesh component
	Mesh * ImportMesh(std::string & aFilename);
	// Uses Assimp importer to get mesh positions from file and returns it in a DebugVertex array
	std::vector<DebugVertex> ImportColliderData(std::string & aFilename);

	virtual void OnNotify(Event * aEvent) override;
};