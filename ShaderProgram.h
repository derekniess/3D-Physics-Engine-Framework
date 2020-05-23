#pragma once

class Renderer;

class ShaderProgram
{
	/*-----------------MEMBER VARIABLES------------------*/
private:
	/*--------------------------- SHADER FILE NAME LIST --------------------------------*/
	const char * vertexFile[1];
	const char * fragmentFile[1];
	const char * geometryFile[1];
	/*--------------------------- VERTEX SHADER --------------------------------*/
	GLuint VertexShader;
	std::string VertexSource;
	/*--------------------------- FRAGMENT SHADER --------------------------------*/
	GLuint FragmentShader;
	std::string FragmentSource;
	/*--------------------------- GEOMETRY SHADER --------------------------------*/
	GLuint GeometryShader;
	std::string GeometrySource;
	/*--------------------------- SHADER PROGRAM --------------------------------*/
	GLuint ShaderProgramID;
	/*--------------------------- RENDERER REFERENCE --------------------------------*/
	Renderer & RendererReference;

	/*-----------------MEMBER FUNCTIONS------------------*/
public:
	// Creates, links and validates the Default Shader Program 
	bool CreateDefaultShaderProgram();

	// Creates, links and validates the Debug - Wireframes, normals - Shader Program 
	bool CreateDebugNormalsShaderProgram();

	// Creates, links and validates the Debug - Mesh - Shader Program 
	bool CreateDebugMeshShaderProgram();

	// Creates, links and validates the Billboarding Quads  Shader Program 
	bool CreateBillboardingQuadShaderProgram();

	// Returns status of vertex shader compilation
	bool CheckVertexShaderCompilationStatus(GLuint aShaderID);

	// Returns status of fragment shader compilation
	bool CheckFragmentShaderCompilationStatus(GLuint aShaderID);

	// Returns status of geometry shader compilation
	bool CheckGeometryShaderCompilationStatus(GLuint aShaderID);

	// Returns status of program compilation
	bool CheckProgramCompilationStatus(GLuint aProgramID);

	ShaderProgram(Renderer & render) : RendererReference(render) {}
	virtual ~ShaderProgram();

    inline Renderer & GetRenderer() { return RendererReference; };
	
	GLuint GetShaderProgram() { return ShaderProgramID; }
	
	void Use();
	void Unuse();

private:
	// Default shader programs
	bool CreateDefaultVertexShader();
	bool CreateDefaultFragmentShader();
	bool CreateDefaultGeometryShader();
	
	// Debug wireframe, face and vertex normal rendering shader programs
	bool CreateDebugNormalsVertexShader();
	bool CreateDebugNormalsFragmentShader();
	bool CreateDebugNormalsGeometryShader();

	// Debug mesh shader program
	bool CreateDebugMeshVertexShader();
	bool CreateDebugMeshFragmentShader();

	// Billboarding quads shader program
	bool CreateBillboardingQuadVertexShader();
	bool CreateBillboardingQuadFragmentShader();

};
