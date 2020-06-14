#pragma once
#include "Typedefs.h"
// GLEW
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>

#include "Observer.h"
// Render utilities
#include "ShaderProgram.h"

class Primitive;
class Light;
class Camera;
class Engine;

class Renderer : public Observer
{
	/*----------MEMBER VARIABLES----------*/
public:

	enum PrimitiveDataType
	{
		STATIC,  // Vertex data is immutable for lifespan of primitive
		DYNAMIC, // Vertex data can change over lifespan of primitive
		DataTypeCount
	};

	/*--------------------------- CONSTANTS --------------------------------*/
	const static int MAXIMUM_STATIC_RENDER_OBJECTS = 4096;
	const static int MAXIMUM_DYNAMIC_RENDER_OBJECTS = 1024;
	/*--------------------------- SHADER PROGRAMS --------------------------------*/
	ShaderProgram DefaultShader;
	ShaderProgram DebugNormalsShader;
	ShaderProgram DebugMeshShader;
	ShaderProgram BillboardingQuadsShader;
	ShaderProgram LightSourceShader;
	/*--------------------------- VERTEX ARRAY OBJECTS --------------------------------*/
	GLuint * StaticVAOList[MAXIMUM_STATIC_RENDER_OBJECTS];
	GLuint * DynamicVAOList[MAXIMUM_DYNAMIC_RENDER_OBJECTS];
	/*--------------------------- VERTEX BUFFER OBJECTS --------------------------------*/
	GLuint * StaticVBOList[MAXIMUM_STATIC_RENDER_OBJECTS];
	GLuint * DynamicVBOList[MAXIMUM_DYNAMIC_RENDER_OBJECTS];
	/*--------------------------- ELEMENT BUFFER OBJECTS --------------------------------*/
	GLuint * EABList[MAXIMUM_STATIC_RENDER_OBJECTS];
	/*--------------------------- TEXTURE BUFFER OBJECTS --------------------------------*/
	GLuint * TBOList[MAXIMUM_STATIC_RENDER_OBJECTS];
	/*--------------------------- REGISTRIES --------------------------------*/
	// Holds the list of slots available for static objects to bind VAO and VBOs - false is 'empty', true is 'currently in use'
	bool StaticObjectRegistry[MAXIMUM_STATIC_RENDER_OBJECTS];
	// Holds the list of slots available for dynamic objects to bind VAO and VBOs - false is 'empty', true is 'currently in use'
	bool DynamicObjectRegistry[MAXIMUM_DYNAMIC_RENDER_OBJECTS];
	/*------------------------------- ENGINE REFERENCE -------------------------------*/
	Engine & EngineHandle;
	/*--------------------------- MATRICES --------------------------------*/
	glm::mat4 Projection;
	glm::mat4 View;

	// The horizontal Field of View, in degrees : the amount of "zoom". Think "camera lens". Usually between 90� (extra wide) and 30� (quite zoomed in)
	float FieldOfView = 45.0f;

	// Debug Primitives
	Primitive * DebugArrowPrimitive;
	Primitive * DebugQuadPrimitive;

	// List of render components
	std::vector<Primitive *> RenderList;
	// List of light components
	std::vector<Light *> LightList;
	// Holds the number of currently active/bound textures
	GLuint TextureCount;
	// The thickness of debug wireframe lines
	static int WireframeThickness;
	// The thickness of debug line loops
	static int LineLoopThickness;
	// TODO [@Derek] : Later on use an array of unique ptrs to cameras owned by renderer, 
	// Active camera at any time is pointed to by this pointer
	Camera * pActiveCamera;

	/*----------MEMBER FUNCTIONS----------*/
public:
	Renderer(Engine & aEngine) :EngineHandle(aEngine),
		DefaultShader(*this), 
		DebugNormalsShader(*this),
		DebugMeshShader(*this),
		BillboardingQuadsShader(*this),
		LightSourceShader(*this),
		TextureCount(0)
	{}

	virtual ~Renderer()
	{
		for (int i = 0; i < MAXIMUM_STATIC_RENDER_OBJECTS; i++)
		{
			// BUFFER DELETION
			glDeleteBuffers(1, StaticVBOList[i]);
			glDeleteBuffers(1, EABList[i]);
			// TEXTURE DELETION
			glDeleteTextures(1, TBOList[i]);
			// VERTEX ARRAY DELETION
			glDeleteVertexArrays(1, StaticVAOList[i]);
		}

		for (int i = 0; i < MAXIMUM_DYNAMIC_RENDER_OBJECTS; i++)
		{
			// BUFFER DELETION
			glDeleteBuffers(1,DynamicVBOList[i]);
			// VERTEX ARRAY DELETION
			glDeleteVertexArrays(1, DynamicVAOList[i]);
		}
	}

	// Getters
	inline GLuint const & GetActiveShaderProgram() { return DefaultShader.GetShaderProgram(); }
	inline int GetRenderListSize() { return (int)RenderList.size(); }
	inline int GetTextureCount() { return TextureCount; }
	
	Engine & GetEngine() { return EngineHandle; }

	// Setters
	inline void SetActiveCamera(Camera * aCameraPtr) { pActiveCamera = aCameraPtr; }

	void InititalizeRenderer();
	
	/* --- Primitive handling functions --- */
	// Converts static primitive to dynamic primitive
	void ConvertStaticToDynamic(Primitive * aPrimitive);
	// Gives a primitive a VAO & VBO to use, adds to list of render objects
	void RegisterPrimitive(Primitive * aNewPrimitive);
	// Called by RegisterPrimitive()
	void RegisterStaticPrimitive(Primitive * aNewPrimitive);
	void RegisterDynamicPrimitive(Primitive * aNewPrimitive);

	// Removes a primitive from Dynamic/Static list
	void DeregisterPrimitive(Primitive * aOldPrimitive);
	// Called by DeregisterPrimitive()
	void DeregisterStaticPrimitive(Primitive * aOldPrimitive);
	void DeregisterDynamicPrimitive(Primitive * aOldPrimitive);

	// Create the debug arrow primitive and save it for later
	void CreateDebugArrowPrimitive();
	// Create the debug quad primitive and save it for later
	void CreateDebugQuadPrimitive();
	
	/* --- Light handling functions --- */
	void RegisterLight(Light * aNewLight);

	// Binds texture at requested ID to primitive
	bool BindTexture(Primitive * aPrimitive, int aTextureID);
	
	void Render();
	// Main pass render functions
	void MainRenderPass();

	// Debug pass render function
	void DebugRenderPass();
	void RenderDebugWireframes(GLint aMVPAttributeIndex);
	void RenderDebugNormals(GLint aMVPAttributeIndex);
	void RenderDebugArrows(GLint aMVPAttributeIndex);
	void RenderDebugLineLoops(GLint aMVPAttributeIndex);
	void RenderBillboardingQuads(GLint aModelAttributeIndex, GLint aViewAttributeIndex, GLint aProjectionAttributeIndex, GLint aBillboardModeAttributeIndex);
	
	void RenderLightSources(GLint aMVPAttributeIndex);

	// Utility functions
	inline void SetRenderModeWireframe()
	{
		glLineWidth((GLfloat)WireframeThickness);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	inline void SetRenderModeFill()
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	static void check_gl_error_render()
	{
		GLenum err(glGetError());
		using namespace std;
		while (err != GL_NO_ERROR) {
			string error;

			switch (err) {
			case GL_INVALID_OPERATION:      error = "INVALID_OPERATION";      break;
			case GL_INVALID_ENUM:           error = "INVALID_ENUM";           break;
			case GL_INVALID_VALUE:          error = "INVALID_VALUE";          break;
			case GL_OUT_OF_MEMORY:          error = "OUT_OF_MEMORY";          break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
			}

			cerr << "GL_" << error.c_str() << endl;
			err = glGetError();
		}
	}

	virtual void OnNotify(Event * aEvent) override;
	
};