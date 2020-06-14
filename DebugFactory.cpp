#include "DebugFactory.h"
#include "Engine.h"
#include "GameObjectFactory.h"
#include "Renderer.h"



DebugFactory::DebugFactory(Engine & aEngine) : EngineHandle(aEngine)
{
}

DebugFactory::~DebugFactory()
{
}

// Debug objects are non-persistent so they don't need to be on the Render List
void DebugFactory::RegisterDebugQuad(Quad & aQuad)
{
	DebugQuadsStack.push_back(aQuad);
}

void DebugFactory::RegisterDebugArrow(Arrow & aArrow)
{
	DebugArrowsStack.push_back(aArrow);
}

void DebugFactory::RegisterDebugLineLoop(LineLoop & aLineLoop)
{
	Renderer & renderer = EngineHandle.GetRenderer();
	for (int i = 0; i < renderer.MAXIMUM_STATIC_RENDER_OBJECTS; ++i)
	{
		// Allocate using first free slot
		if (renderer.StaticObjectRegistry[i] == false)
		{
			// If slot is found, use its VAO and VBO and mark it as in use
			aLineLoop.VAO = *renderer.StaticVAOList[i];
			aLineLoop.VBO = *renderer.StaticVBOList[i];
			aLineLoop.RegistryID = i;
			renderer.StaticObjectRegistry[i] = true;
			break;
		}
	}
	aLineLoop.BindVertexData();
	DebugLineLoopsStack.push_back(aLineLoop);
}

void DebugFactory::OnNotify(Event * aEvent)
{
	// Check if this is an Engine event
	EngineEvent * engineEvent = nullptr;
	engineEvent = dynamic_cast<EngineEvent *>(aEvent);

	if (engineEvent)
	{
		if (engineEvent->EventID == EngineEvent::EventList::ENGINE_INIT)
		{
			/*---------- MINKOWSKI DIFFERENCE INIT ----------*/
			MinkowskiDifference = EngineHandle.GetGameObjectFactory().SpawnGameObject();
			std::vector<Vertex> MinkowskiDifferenceVertices;
			Primitive * minkowskiPrimitive = EngineHandle.GetGameObjectFactory().SpawnComponent<Primitive>();
			minkowskiPrimitive->ePrimitiveDataType = Renderer::DYNAMIC;
			MinkowskiDifference->AddComponent(minkowskiPrimitive);
			EngineHandle.GetRenderer().RegisterPrimitive(minkowskiPrimitive);
		}
		return;
	}

}