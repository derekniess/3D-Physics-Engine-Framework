#include "Component.h"

const char * Component::ComponentTypeName[Component::ComponentType::TypeCount] =
{
	"Transform",
	"Primitive",
	"Physics",
	"Controller",
	"Script",
	"Collider",
	"Light"
};