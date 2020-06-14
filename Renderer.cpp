// C++ header files
#include <iostream>
#include <fstream>
#include <list>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "Renderer.h"
#include "Engine.h"
#include "EngineStateManager.h"
#include "GameObjectFactory.h"
#include "DebugFactory.h"
#include "WindowManager.h"

#include "GameObject.h"
#include "Primitive.h"
#include "Light.h"
#include "Texture.h"
#include "Camera.h"

#include "UtilityFunctions.h"
// Render Utilities
#include "Arrow.h"
#include "LineLoop.h"
#include "Quad.h"

int Renderer::WireframeThickness = 4;
int Renderer::LineLoopThickness = 6;

void Renderer::ConvertStaticToDynamic(Primitive * aPrimitive)
{
	// Remove object from static list and add it to dynamic list
	if (aPrimitive->ePrimitiveDataType == PrimitiveDataType::STATIC)
	{
		DeregisterStaticPrimitive(aPrimitive);
		RegisterDynamicPrimitive(aPrimitive);
	}
}

void Renderer::RegisterPrimitive(Primitive * aNewPrimitive)
{
	// Assign new primitive a VAO and VBO, Static and Dynamic VAOs/VBOs are stored in different arrays (hence they are assigned separately)
	if (aNewPrimitive->ePrimitiveDataType == PrimitiveDataType::STATIC)
	{
		RegisterStaticPrimitive(aNewPrimitive);
	}
	else if (aNewPrimitive->ePrimitiveDataType == PrimitiveDataType::DYNAMIC)
	{
		RegisterDynamicPrimitive(aNewPrimitive);
	}
	RenderList.push_back(aNewPrimitive);
	aNewPrimitive->bIsBound = true;
	aNewPrimitive->TextureRequest.AddObserver(this);
}

void Renderer::RegisterStaticPrimitive(Primitive * aNewPrimitive)
{
	for (int i = 0; i < MAXIMUM_STATIC_RENDER_OBJECTS; ++i)
	{
		// Allocate using first free slot
		if (StaticObjectRegistry[i] == false)
		{
			// If a free slot is found, assign its VAO and VBO to the primitive and mark slot as filled
			aNewPrimitive->VAO = *StaticVAOList[i];
			aNewPrimitive->VBO = *StaticVBOList[i];
			aNewPrimitive->PrimitiveSlot = i;
			StaticObjectRegistry[i] = true;
			return;
		}
	}
}

void Renderer::RegisterDynamicPrimitive(Primitive * aNewPrimitive)
{
	for (int i = 0; i < MAXIMUM_DYNAMIC_RENDER_OBJECTS; ++i)
	{
		// Allocate using first free slot
		if (DynamicObjectRegistry[i] == false)
		{
			// If a free slot is found, assign its VAO and VBO to the primitive and mark slot as filled
			aNewPrimitive->VAO = *DynamicVAOList[i];
			aNewPrimitive->VBO = *DynamicVBOList[i];
			aNewPrimitive->PrimitiveSlot = i;
			DynamicObjectRegistry[i] = true;
			return;
		}
	}
}

void Renderer::DeregisterPrimitive(Primitive * aOldPrimitive)
{
	std::vector<Primitive *>::iterator oldPrimitiveLocation = std::find(RenderList.begin(), RenderList.end(), aOldPrimitive);
	// If primitive exists in render list
	if (oldPrimitiveLocation != RenderList.end())
	{
		if (aOldPrimitive->ePrimitiveDataType == PrimitiveDataType::STATIC)
		{
			DeregisterStaticPrimitive(aOldPrimitive);
		}
		else if (aOldPrimitive->ePrimitiveDataType == PrimitiveDataType::DYNAMIC)
		{
			DeregisterDynamicPrimitive(aOldPrimitive);
		}
		// Swaps the old primitive with end of list and pops it
		std::swap(*oldPrimitiveLocation, RenderList.back());
		RenderList.pop_back();
	}
}

void Renderer::DeregisterStaticPrimitive(Primitive * aOldPrimitive)
{
	// Free slot in static object registry
	StaticObjectRegistry[aOldPrimitive->PrimitiveSlot] = false;
}

void Renderer::DeregisterDynamicPrimitive(Primitive * aOldPrimitive)
{
	// Free slot in dynamic object registry
	DynamicObjectRegistry[aOldPrimitive->PrimitiveSlot] = false;
}

void Renderer::CreateDebugArrowPrimitive()
{
	DebugArrowPrimitive = EngineHandle.GetGameObjectFactory().SpawnComponent<Primitive>();
	// Registered as a static primitive
	RegisterPrimitive(DebugArrowPrimitive);
	DebugArrowPrimitive->bIsDebug = true;
	Arrow newArrow(glm::vec3(0), glm::vec3(1));
	newArrow.VAO = DebugArrowPrimitive->VAO;
	newArrow.VBO = DebugArrowPrimitive->VBO;
	newArrow.BindVertexData();
}

void Renderer::CreateDebugQuadPrimitive()
{
	DebugQuadPrimitive = EngineHandle.GetGameObjectFactory().SpawnComponent<Primitive>();
	// Registered as a static primitive
	RegisterPrimitive(DebugQuadPrimitive);
	DebugQuadPrimitive->bIsDebug = true;
	Quad newQuad(glm::vec3(0));
	newQuad.VAO = DebugQuadPrimitive->VAO;
	newQuad.VBO = DebugQuadPrimitive->VBO;
	newQuad.BindVertexData();
}

void Renderer::RegisterLight(Light * aNewLight)
{
	aNewLight->LightSlot = (int)LightList.size();
	LightList.push_back(aNewLight);
}


void Renderer::InititalizeRenderer()
{
	/*--------------------------- GLEW INIT -------------------------------*/
	glewExperimental = GL_TRUE;
	if (glewInit())
	{
		int x;
		std::cout << "GLEW failed to initialize!\n";
		std::cin >> x;
		exit(1); // or handle the error in a nicer way, TODO : print to log or something
	}
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	std::cout << "Renderer: " << renderer << std::endl;
	std::cout << "OpenGL version:" << version << std::endl;
	/*---------- BUFFER ALLOCATION ----------*/
	// Allocates the vertex arrays and  vertex buffers for static objects, as well as element and texture buffers
	for (int i = 0; i < MAXIMUM_STATIC_RENDER_OBJECTS; i++)
	{
		StaticVAOList[i] = new GLuint;
		glGenVertexArrays(1, StaticVAOList[i]);
		StaticVBOList[i] = new GLuint;
		glGenBuffers(1, StaticVBOList[i]);
		EABList[i] = new GLuint;
		glGenBuffers(1, EABList[i]);
		TBOList[i] = new GLuint;
		glGenTextures(1, TBOList[i]);
		StaticObjectRegistry[i] = false;
	}

	// Allocates the vertex arrays and  vertex buffers for dynamic objects
	for (int i = 0; i < MAXIMUM_DYNAMIC_RENDER_OBJECTS; i++)
	{
		DynamicVAOList[i] = new GLuint;
		glGenVertexArrays(1, DynamicVAOList[i]);
		DynamicVBOList[i] = new GLuint;
		glGenBuffers(1, DynamicVBOList[i]);
		DynamicObjectRegistry[i] = false;
	}

	/*---------- OPEN GL SETTINGS ----------*/
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);		// Depth test
	glEnable(GL_BLEND);				// Alpha Blending
	glDisable(GL_CULL_FACE);		// Backface Culling

	/*---------- SHADER CREATION ----------*/
	// Create and use default shader program
	DefaultShader.CreateDefaultShaderProgram();
	DefaultShader.Use();
	// Create debug normals shader program
	DebugNormalsShader.CreateDebugNormalsShaderProgram();
	// Create debug mesh shader program
	DebugMeshShader.CreateDebugMeshShaderProgram();
	// Create debug quads shader program
	BillboardingQuadsShader.CreateBillboardingQuadShaderProgram();
	// Create light source shader program
	LightSourceShader.CreateLightSourceShaderProgram();
	/*---------- PRIMITIVE CREATION ----------*/
	CreateDebugArrowPrimitive();
	CreateDebugQuadPrimitive();
}

void Renderer::OnNotify(Event * aEvent)
{
	// Check if this is an Engine event
	EngineEvent * engineEvent = nullptr;
	engineEvent = dynamic_cast<EngineEvent *>(aEvent);

	if (engineEvent)
	{
		if (engineEvent->EventID == EngineEvent::EventList::ENGINE_INIT)
		{
			InititalizeRenderer();
		}
		else if (engineEvent->EventID == EngineEvent::EventList::ENGINE_TICK)
		{
			Render();
		}
		return;
	}

	PrimitiveEvent * primitiveEvent = nullptr;
	primitiveEvent = dynamic_cast<PrimitiveEvent *>(aEvent);

	if (primitiveEvent)
	{
		if (engineEvent->EventID == PrimitiveEvent::EventList::TEXTURE_REQUEST)
		{
			// Already bound textures are not rebound
			if (primitiveEvent->TextureID >= TextureCount)
				BindTexture(primitiveEvent->EventOrigin, primitiveEvent->TextureID);
		}
		return;
	}

	WindowEvent * windowEvent = nullptr;
	windowEvent = dynamic_cast<WindowEvent *>(aEvent);
	if (windowEvent)
	{
		if (windowEvent->EventID == WindowEvent::EventList::WINDOW_RESIZE)
		{

		}
	}
}


void Renderer::Render()
{
	// Update camera values before constructing view matrix
	vector3 cameraPosition = pActiveCamera->GetCameraPosition();
	vector3 cameraTarget = pActiveCamera->GetCameraLookDirection();
	vector3 upVector = pActiveCamera->GetCameraUpDirection();

	/*-------------------------------- VIEW MATRIX -------------------------------*/
	View = glm::lookAt(
		cameraPosition,
		cameraTarget,
		upVector
	);

	// Update field of view before constructing projection matrix
	FieldOfView += InputManager::GetScrollDelta().y * pActiveCamera->GetSensitivity();
	/*-------------------------------- PROJECTION MATRIX -------------------------------*/
	Projection = glm::perspective(
		FieldOfView, // The horizontal Field of View, in degrees : the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
		4.f/3, // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
		0.1f,        // Near clipping plane. Keep as big as possible, or you'll get precision issues.
		10000.0f       // Far clipping plane. Keep as little as possible.
	);

	// Sets background color
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	// Clear color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw all game objects
	MainRenderPass();

	// Draw all debug objects
	DebugRenderPass();

}

void Renderer::MainRenderPass()
{
	Light * pBaseLight = LightList[0];
	vector3 baseLightPosition = pBaseLight->GetOwner()->GetComponent<Transform>()->GetPosition();
	vector3 baseLightColor = pBaseLight->Color;
	vector3 cameraPosition = pActiveCamera->GetCameraPosition();
	/*-------------------------------- REGULAR MESH RENDER-------------------------------*/
	DefaultShader.Use();

	// Get uniform locations
	GLint glMVPAttributeIndex = DefaultShader.GetUniformLocation("ModelViewProjectionMatrix");
	GLint glLightColorAttributeIndex = DefaultShader.GetUniformLocation("LightColor");
	GLint glLightPositionAttributeIndex = DefaultShader.GetUniformLocation("LightPosition");
	GLint glModelAttributeIndex = DefaultShader.GetUniformLocation("Model");
	GLint glViewPositionAttributeIndex = DefaultShader.GetUniformLocation("ViewPosition");

	// Set light color value
	glUniform3f(glLightColorAttributeIndex, baseLightColor.x, baseLightColor.y, baseLightColor.z);
	// Set light position value
	glUniform3f(glLightPositionAttributeIndex, baseLightPosition.x, baseLightPosition.y, baseLightPosition.z);
	// Set camera position value
	glUniform3f(glViewPositionAttributeIndex, cameraPosition.x, cameraPosition.y, cameraPosition.z);

	// Wireframe draw check
	if (EngineHandle.GetEngineStateManager().bRenderModeWireframe)
		SetRenderModeWireframe();
	else
		SetRenderModeFill();

	/*-------- STATIC MESH RENDER ----------*/
	for (int i = 0; i < RenderList.size(); ++i)
	{
		Transform * transform = nullptr;
		Primitive * primitive = RenderList[i];

		// Skip any unbound primitives or debug primitives in main render pass
		if (!primitive->bIsBound || primitive->bIsDebug)
			continue;
		// Skip any non-static meshes
		if(primitive->ePrimitiveDataType != STATIC)
			continue;
		// Use wireframe mode for wireframe primitives
		if (primitive->bIsWireframePrimitive)
			SetRenderModeWireframe();
		else if (!EngineHandle.GetEngineStateManager().bRenderModeWireframe)
			SetRenderModeFill();

		GameObject * renderObject = primitive->GetOwner(); 
		transform = renderObject->GetComponent<Transform>();
		// Calculate the MVP matrix and set the matrix uniform
		matrix4 mvp;
		matrix4 model;
		matrix4 translate = glm::translate(transform->GetPosition());
		matrix4 rotate = glm::mat4_cast(transform->GetRotation());
		matrix4 scale = glm::scale(transform->GetScale());
		model = translate * rotate * scale;
		mvp = Projection * View * model;
		glEnableClientState(GL_VERTEX_ARRAY);
		glUniformMatrix4fv(glMVPAttributeIndex, 1, GL_FALSE, &mvp[0][0]);
		glUniformMatrix4fv(glModelAttributeIndex, 1, GL_FALSE, &model[0][0]);
		check_gl_error_render();
		// Bind TBO
		glBindTexture(GL_TEXTURE_2D, primitive->TBO);
		// Bind VAO
		glBindVertexArray(*StaticVAOList[primitive->PrimitiveSlot]);
		check_gl_error_render();
		glDrawArrays(GL_TRIANGLES, 0, primitive->PrimitiveSize/sizeof(Vertex));
		check_gl_error_render();
		assert(glGetError() == GL_NO_ERROR);
		// Unbind VAO when done
		glBindVertexArray(0);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
	/*-------- DYNAMIC MESH RENDER ----------*/
	// Render Minkowski Difference
	if (EngineHandle.GetEngineStateManager().bShouldRenderMinkowskiDifference)
	{
		Mesh * shape1 = static_cast<Mesh *>(RenderList[4]);
		Mesh * shape2 = static_cast<Mesh *>(RenderList[5]);
		std::vector<Vertex> MinkowskiDifferenceVertices;
		Utility::CalculateMinkowskiDifference(MinkowskiDifferenceVertices, shape1, shape2);
		EngineHandle.GetDebugFactory().MinkowskiDifference->GetComponent<Primitive>()->BindVertexData(MinkowskiDifferenceVertices);
	}
	else
	{
		EngineHandle.GetDebugFactory().MinkowskiDifference->GetComponent<Primitive>()->Debuffer();
	}
	for (int i = 0; i < RenderList.size(); ++i)
	{
		Transform * transform = nullptr;
		Primitive * primitive = RenderList[i];

		// Skip any unbound primitives or debug primitives in main render pass
		if (!primitive->bIsBound || primitive->bIsDebug)
			continue;
		// Skip any non-dynamic meshes
		if (primitive->ePrimitiveDataType != DYNAMIC)
			continue;

		GameObject * renderObject = primitive->GetOwner();
		transform = renderObject->GetComponent<Transform>();
		// Calculate the MVP matrix and set the matrix uniform
		matrix4 mvp;
		matrix4 model;
		matrix4 translate = glm::translate(transform->GetPosition());
		matrix4 rotate = glm::mat4_cast(transform->GetRotation());
		matrix4 scale = glm::scale(transform->GetScale());
		model = translate * rotate * scale;
		mvp = Projection * View * model;
		glEnableClientState(GL_VERTEX_ARRAY);
		glUniformMatrix4fv(glMVPAttributeIndex, 1, GL_FALSE, &mvp[0][0]);
		check_gl_error_render();
		// Bind TBO
		glBindTexture(GL_TEXTURE_2D, primitive->TBO);
		// Bind VAO
		glBindVertexArray(*DynamicVAOList[primitive->PrimitiveSlot]);
		check_gl_error_render();
		glDrawArrays(GL_TRIANGLES, 0, primitive->PrimitiveSize / sizeof(Vertex));
		check_gl_error_render();
		assert(glGetError() == GL_NO_ERROR);
		// Unbind VAO when done
		glBindVertexArray(0);
		glDisableClientState(GL_VERTEX_ARRAY);
	}

	DefaultShader.Unuse();

	/*-------- BILLBOARDING QUADS RENDER ----------*/
	BillboardingQuadsShader.Use();

	GLint glModelMatrixAttributeIndex, glViewMatrixAttributeIndex, glProjectionMatrixAttributeIndex, glBillboardModeAttributeIndex;
	glModelMatrixAttributeIndex = BillboardingQuadsShader.GetUniformLocation("ModelMatrix");
	glViewMatrixAttributeIndex = BillboardingQuadsShader.GetUniformLocation("ViewMatrix");
	glProjectionMatrixAttributeIndex = BillboardingQuadsShader.GetUniformLocation("ProjectionMatrix");
	glBillboardModeAttributeIndex = BillboardingQuadsShader.GetUniformLocation("BillboardMode");
	RenderBillboardingQuads(glModelMatrixAttributeIndex, glViewMatrixAttributeIndex, glProjectionMatrixAttributeIndex, glBillboardModeAttributeIndex);
	
	BillboardingQuadsShader.Unuse();

	/*-------- LIGHT SOURCES RENDER ----------*/
	LightSourceShader.Use();

	glMVPAttributeIndex = LightSourceShader.GetUniformLocation("ModelViewProjectionMatrix");
	RenderLightSources(glMVPAttributeIndex);
	
	LightSourceShader.Unuse();

}

void Renderer::RenderLightSources(GLint aMVPAttributeIndex)
{
	for (int i = 0; i < LightList.size(); ++i)
	{
		Transform * transform = nullptr;
		Light * light = LightList[i];
		Primitive * lightMesh = light->GetOwner()->GetComponent<Primitive>();

		GameObject * renderObject = light->GetOwner();
		transform = renderObject->GetComponent<Transform>();

		// Calculate the MVP matrix and set the matrix uniform
		matrix4 mvp;
		matrix4 model;
		matrix4 translate = glm::translate(transform->GetPosition());
		matrix4 rotate = glm::mat4_cast(transform->GetRotation());
		matrix4 scale = glm::scale(transform->GetScale());
		model = translate * rotate * scale;
		mvp = Projection * View * model;
		glEnableClientState(GL_VERTEX_ARRAY);
		glUniformMatrix4fv(aMVPAttributeIndex, 1, GL_FALSE, &mvp[0][0]);
		check_gl_error_render();
		// Bind VAO
		glBindVertexArray(*StaticVAOList[lightMesh->PrimitiveSlot]);
		check_gl_error_render();
		glDrawArrays(GL_TRIANGLES, 0, lightMesh->PrimitiveSize / sizeof(Vertex));
		check_gl_error_render();
		assert(glGetError() == GL_NO_ERROR);
		// Unbind VAO when done
		glBindVertexArray(0);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
}

void Renderer::DebugRenderPass()
{
	GLint glMVPAttributeIndex;
	/*-------------------------------- DEBUG MESH RENDER-------------------------------*/
	if (EngineHandle.GetEngineStateManager().bShouldRenderCollidersAndNormals)
	{	
		// Render wireframes
		DefaultShader.Use();
		glMVPAttributeIndex = DefaultShader.GetUniformLocation("ModelViewProjectionMatrix");
		RenderDebugWireframes(glMVPAttributeIndex);
		DefaultShader.Unuse();

		// Render normals
		DebugNormalsShader.Use();
		glMVPAttributeIndex = DebugNormalsShader.GetUniformLocation("ModelViewProjectionMatrix");
		RenderDebugNormals(glMVPAttributeIndex);
		DebugNormalsShader.Unuse();
	}

	// Render debug arrows
	DebugMeshShader.Use();
	glMVPAttributeIndex = DebugMeshShader.GetUniformLocation("ModelViewProjectionMatrix");
	RenderDebugArrows(glMVPAttributeIndex);

	// Render debug line loops
	RenderDebugLineLoops(glMVPAttributeIndex);
	DebugMeshShader.Unuse();

}

void Renderer::RenderDebugWireframes(GLint aMVPAttributeIndex)
{
	// Render all objects, scaled up and in wireframe mode
	SetRenderModeWireframe();
	for (int i = 0; i < RenderList.size(); ++i)
	{
		Transform * transform = nullptr;
		Primitive * primitive = RenderList[i];

		// If unbound or a debug object (don't render debug for debug, that would just be silly)
		if (!primitive->bIsBound || primitive->bIsDebug)
			continue;
		if (primitive->bShouldRenderWireframe)
		{
			GameObject * renderObject = primitive->GetOwner();
			transform = renderObject->GetComponent<Transform>();
			// Calculate the MVP matrix and set the matrix uniform
			matrix4 mvp;
			matrix4 model;
			matrix4 translate = glm::translate(transform->GetPosition());
			matrix4 rotate = glm::mat4_cast(transform->GetRotation());
			matrix4 scale = glm::scale(transform->GetScale() * 1.25f);
			model = translate * rotate * scale;
			mvp = Projection * View * model;
			glEnableClientState(GL_VERTEX_ARRAY);
			glUniformMatrix4fv(aMVPAttributeIndex, 1, GL_FALSE, &mvp[0][0]);
			check_gl_error_render();
			glLineWidth((GLfloat)WireframeThickness);
			// Bind VAO
			glBindVertexArray(*StaticVAOList[i]);
			check_gl_error_render();
			glDrawArrays(GL_TRIANGLES, 0, primitive->PrimitiveSize / sizeof(Vertex));
			check_gl_error_render();
			assert(glGetError() == GL_NO_ERROR);
			// Unbind VAO when done
			glBindVertexArray(0);
			glDisableClientState(GL_VERTEX_ARRAY);
		}
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Renderer::RenderDebugNormals(GLint aMVPAttributeIndex)
{
	// Render Normals
	for (int i = 0; i < RenderList.size(); ++i)
	{
		Transform * transform = nullptr;
		Primitive * primitive = RenderList[i];

		// If unbound or a debug object (don't render debug for debug, that would just be silly)
		if (!primitive->bIsBound || primitive->bIsDebug)
			continue;
		if (primitive->bShouldRenderWireframe)
		{
			GameObject * renderObject = primitive->GetOwner();
			transform = renderObject->GetComponent<Transform>();
			// Calculate the MVP matrix and set the matrix uniform
			matrix4 mvp;
			matrix4 model;
			matrix4 translate = glm::translate(transform->GetPosition());
			matrix4 rotate = glm::mat4_cast(transform->GetRotation());
			matrix4 scale = glm::scale(transform->GetScale() * 1.25f);
			model = translate * rotate * scale;
			mvp = Projection * View * model;
			glEnableClientState(GL_VERTEX_ARRAY);
			glUniformMatrix4fv(aMVPAttributeIndex, 1, GL_FALSE, &mvp[0][0]);
			check_gl_error_render();
			glLineWidth(1);
			// Bind VAO
			glBindVertexArray(*StaticVAOList[i]);
			check_gl_error_render();
			glDrawArrays(GL_TRIANGLES, 0, primitive->PrimitiveSize / sizeof(Vertex));
			check_gl_error_render();
			assert(glGetError() == GL_NO_ERROR);
			// Unbind VAO when done
			glBindVertexArray(0);
			glDisableClientState(GL_VERTEX_ARRAY);
		}
	}
}

void Renderer::RenderDebugArrows(GLint aMVPAttributeIndex)
{
	glm::mat4 projectionView = Projection * View;
	SetRenderModeFill();

	DebugFactory & debugFactory = EngineHandle.GetDebugFactory();
	// Draw all arrows that have been registered 
	for (int i = 0; i < debugFactory.DebugArrowsStack.size(); ++i)
	{
		Arrow & debugArrow = debugFactory.DebugArrowsStack[i];
		matrix4 model;
		matrix4 translate = glm::translate(debugArrow.PointA);
		// Create rotation matrix using the direction the arrow points in
		vector3 normal = debugArrow.PointA - debugArrow.PointB;
		normal = glm::normalize(normal);
		matrix4 rotate = glm::orientation(normal, glm::vec3(1, 0, 0));
		matrix4 scale = glm::scale(glm::vec3(debugArrow.Scale));
		model = translate * rotate * scale;
		projectionView = projectionView * model;
		glEnableClientState(GL_VERTEX_ARRAY);
		glUniformMatrix4fv(aMVPAttributeIndex, 1, GL_FALSE, &projectionView[0][0]);
		check_gl_error_render();
		glBindVertexArray(DebugArrowPrimitive->VAO);
		check_gl_error_render();
		glDrawArrays(GL_TRIANGLES, 0, debugArrow.VertexCount);
		check_gl_error_render();
		glBindVertexArray(0);
		glDisableClientState(GL_VERTEX_ARRAY);

	}
	debugFactory.DebugArrowsStack.clear();
}

void Renderer::RenderDebugLineLoops(GLint aMVPAttributeIndex)
{
	// Don't need model matrix as all points are in world space already
	matrix4 projectionView = Projection * View;

	DebugFactory & debugFactory = EngineHandle.GetDebugFactory();
	// Draw all Line Loops that have been registered 
	for (int i = 0; i < debugFactory.DebugLineLoopsStack.size(); ++i)
	{
		LineLoop & lineLoop = debugFactory.DebugLineLoopsStack[i];
		glEnableClientState(GL_VERTEX_ARRAY);
		glUniformMatrix4fv(aMVPAttributeIndex, 1, GL_FALSE, &projectionView[0][0]);
		check_gl_error_render();
		glBindVertexArray(lineLoop.VAO);
		check_gl_error_render();
		glLineWidth((GLfloat)LineLoopThickness);
		glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)debugFactory.DebugLineLoopsStack[i].LineLoopVertices.size());
		check_gl_error_render();
		// Unbuffer the data 
		glBindBuffer(GL_ARRAY_BUFFER, lineLoop.VBO);
		glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW);
		glBindVertexArray(0);
		glDisableClientState(GL_VERTEX_ARRAY);
		// Free the slot in the static registry
		StaticObjectRegistry[lineLoop.RegistryID] = false;
	}
	debugFactory.DebugLineLoopsStack.clear();
}

void Renderer::RenderBillboardingQuads(GLint aModelAttributeIndex, GLint aViewAttributeIndex, GLint aProjectionAttributeIndex, GLint aBillboardModeAttributeIndex)
{
	SetRenderModeFill();
	// Set projection and view matrices
	glUniformMatrix4fv(aProjectionAttributeIndex, 1, GL_FALSE, &Projection[0][0]);
	glUniformMatrix4fv(aViewAttributeIndex, 1, GL_FALSE, &View[0][0]);
	// Use cylindrical billboarding
	glUniform1i(aBillboardModeAttributeIndex, 0);

	DebugFactory & debugFactory = EngineHandle.GetDebugFactory();
	// Draw all Quads that have been registered 
	for (int i = 0; i < debugFactory.DebugQuadsStack.size(); ++i)
	{
		matrix4 model;
		matrix4 translate = glm::translate(debugFactory.DebugQuadsStack[i].WorldPosition);
		matrix4 scale = glm::scale(vector3(debugFactory.DebugQuadsStack[i].Scale));
		model = translate * scale;
		glEnableClientState(GL_VERTEX_ARRAY);
		glUniformMatrix4fv(aModelAttributeIndex, 1, GL_FALSE, &model[0][0]);
		check_gl_error_render();
		glBindVertexArray(DebugQuadPrimitive->VAO);
		check_gl_error_render();
		glDrawArrays(GL_TRIANGLES, 0, debugFactory.DebugQuadsStack[i].VertexCount);
		check_gl_error_render();
		glBindVertexArray(0);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
	debugFactory.DebugQuadsStack.clear();
}



// aPrimitive : Primitive the texture is being assigned to
// aTextureID : ID of the texture in the resource manager
// Binds a texture to the image data
bool Renderer::BindTexture(Primitive * aPrimitive, int aTextureID)
{
	Texture * assignedTexture = EngineHandle.GetResourceManager().GetTexture(aTextureID);
	
	GLsizei width = assignedTexture->GetWidth();
	GLsizei height = assignedTexture->GetHeight();

	// Sets the active texture unit using the enum values
	glActiveTexture(GL_TEXTURE0 + TextureCount);

	// Sets what type of texture it is and the texture buffer it is bound to
	glBindTexture(GL_TEXTURE_2D, *TBOList[TextureCount]);

	aPrimitive->TBO = *TBOList[TextureCount];

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, assignedTexture->GetPixels());
	glGenerateMipmap(GL_TEXTURE_2D);

	glUniform1i(glGetUniformLocation(DefaultShader.GetShaderProgram(), "Texture"), aPrimitive->TBO);
	// Sets the texture uniform in the shader
	check_gl_error_render();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	TextureCount++;

	return true;
}
