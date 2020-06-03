#pragma once
#include <GL/glew.h>
// Base class for all components that can implement scene geometry or have collision
// A primitive is also used for geometry defined within the engine (debug shapes etc)
// Use/Derive a Primitive component when defining new custom types of geometry
// TODO : [@Derek] - Make a PrimitiveFactory to create and supply basic shapes (cubes, capsules, spheres, etc.)
#include "Component.h"
#include "Subject.h"
#include "Vertex.h"

#include "Renderer.h"

class Primitive;

class PrimitiveEvent : public Event
{
public:
	enum EventList
	{
		TEXTURE_REQUEST,
		PrimitiveEventCount
	};
	EventList EventID;
	Primitive * EventOrigin;
	unsigned int TextureID;

	PrimitiveEvent() {};
	~PrimitiveEvent() {};
};

class Primitive : public Component
{
	/*----------ENUMS----------*/
public:
	enum PrimitiveType
	{
		SPRITE,
		MESH,
		PrimitiveCount
	};

	/*----------MEMBER VARIABLES----------*/
public:
	int PrimitiveSize;
	int PrimitiveSlot;
	GLuint TBO;
	GLuint VAO;
	GLuint VBO;
	bool bShouldRenderDebug = true;
	std::vector<Vertex> Vertices;

	bool bIsBound;
	bool bIsDebug;
	PrimitiveType ePrimitiveType;
	Renderer::PrimitiveDataType ePrimitiveDataType;

	/*----------MEMBER FUNCTIONS----------*/
	// Called by child classes
	Primitive(PrimitiveType aType) : 
		Component(ComponentType::PRIMITIVE), 
		ePrimitiveType(aType) 
	{}

	// Called by component factory
	Primitive():
		Component(Component::PRIMITIVE)
	{}

	Primitive(Primitive const & CopyPrimitive) : 
		Component(ComponentType::PRIMITIVE) {}

	virtual ~Primitive() {};
	
	static inline ComponentType GetComponentID() { return (ComponentType::PRIMITIVE); }
	static inline const char * GetComponentName() { return ComponentTypeName[ComponentType::PRIMITIVE]; }

	// Sets all vertex colors to a single color
	void SetVertexColorsUniform(glm::vec3 aNewColor);
	// Sets primitive vertex data to provided vertices - should not be called directly
	virtual void BindVertexData(std::vector<Vertex> & aVertexData);
	void ApplyTexture(unsigned int aTextureID);

	// Use move semantics to prevent a copy
	inline void SetVertices(std::vector<Vertex> & aVertexData) {
		Vertices = std::move(aVertexData);
		// Bind VAO with new vertices
		BindVertexData(Vertices);
	}

	// Used to send renderer requests for textures
	Subject TextureRequest;

	void Deserialize(TextFileData aTextData) {};
	void Update();
	// Unbuffers the vertex data
	void Debuffer();
};
