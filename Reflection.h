#pragma once
#include <iostream>
#include <string>

#include "Typedefs.h"
// CRC32 library
#include "crc.h"
/* ------------ TYPE IDENTIFICATION ------------ */

/* EVERY reflected type must specialize this */
// Every type name is registered using this function
template <typename Type>
const char * GetTypeName()
{
	assert(1);
	std::cout << "Type not registered with reflection system!" << std::endl;
}

/* ----- Type Names ----- */
// POD specializations
template <> 
inline const char * GetTypeName <int>() { return "int"; }

template <> 
inline const char * GetTypeName <char>() { return "char"; }

template <> 
inline const char * GetTypeName <float>() { return "float"; }

template <> 
inline const char * GetTypeName <unsigned>() { return "unsigned int"; }

template <> 
inline const char * GetTypeName <std::string>() { return "std::string"; }

// GLM specializations
template <> 
inline const char * GetTypeName <vector2>() { return "vector2"; }

template <> 
inline const char * GetTypeName <vector3>() { return "vector3"; }

template <> 
inline const char * GetTypeName <vector4>() { return "vector4"; }

template <> 
inline const char * GetTypeName <matrix3>() { return "matrix3"; }

template <> 
inline const char * GetTypeName <matrix4>() { return "matrix4"; }

/* ----- Utility Functions ----- */

// Every type name must have an ID associated with it
template <typename Type>
unsigned GetTypeID()
{
	// Calculates the string hash once and then caches it for further use
	static int type_id = crc32(GetTypeName<Type>());
	return type_id;
};

/* ------------ TYPE MEMORY REPRESENTATION ------------ */

// Used for type names and object names
struct Name
{
	unsigned int Hash;
	const char * Text;
};

typedef void(*ConstructObjectFunctor)(void *);
typedef void(*DestructObjectFunctor)(void *);

// The basic type representation
struct Type
{
	// Parent type database
	class TypeDataBase * TypeDBReference;

	// C++ name of the type
	Name InternalName;

	// Pointers to constructor and destructor functions
	ConstructObjectFunctor constructor;
	DestructObjectFunctor destructor;

	// Result of sizeof(ThisType) operation
	size_t Size;
};

// https://stackoverflow.com/questions/222557/what-uses-are-there-for-placement-new
template 
<typename Type> void ConstructObject(void* aObject)
{
	// Use placement new to call constructor
	new (aObject) Type;
}

template 
<typename Type> void DestructObject(void* aObject)
{
	// Explicit call of destructor
	static_cast<Type *>(aObject)->Type::~Type();
}

// Registry of all types in the system with utility functions
class TypeDataBase
{
public:
	template <typename Type>
	Type & CreateType(Name aName);
	//Type * GetType(Name aName);
private:
	typedef std::map<Name, Type *> TypeMap;
	TypeMap Types;
};

template <typename Type>
inline Type & TypeDataBase::CreateType(Name aName)
{
	Type * pType = nullptr;
	TypeMap::iterator typeIterator = Types.find(aName);

	// If type hasn't already been registered with system
	if (typeIterator == Types.end())
	{
		pType = new Type();
		Types.add(std::make_pair(aName, pType));
	}
	else
	{
		pType = typeIterator->second;
	}

	// Apply type properties
	pType->TypeDBReference = this;
	pType->InternalName = &aName;
	pType->constructor = ConstructObject<Type>;
	pType->destructor = DestructObject<Type>;
	return *pType;
}