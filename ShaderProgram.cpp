// C++ header files
#include <iostream>
// GLEW
#include <GL/glew.h>

#include "ShaderProgram.h"
#include "Renderer.h"
// explanation here: https://youtu.be/6ByZWqPzI70?list=PLRwVmtr-pp06qT6ckboaOhnm9FxmzHpbY
bool checkStatus(GLuint objectID, PFNGLGETSHADERIVPROC objPropGetterFunc, PFNGLGETSHADERINFOLOGPROC getInfoLogFunc, GLenum statusType)
{
	GLint status;
	objPropGetterFunc(objectID, statusType, &status);
	if (status != GL_TRUE)
	{
		GLint infoLogLength;
		objPropGetterFunc(objectID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		getInfoLogFunc(objectID, infoLogLength, &bufferSize, buffer);
		std::cout << buffer << std::endl;
		delete[] buffer;
		return false;
	}

	return true;
}

bool checkShaderStatus(GLuint shaderID)
{
	return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}
bool checkProgramStatus(GLuint programID)
{
	return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

void ShaderProgram::Use()
{
	glUseProgram(ShaderProgramID);
}

void ShaderProgram::Unuse()
{
	glUseProgram(0);
}

ShaderProgram::~ShaderProgram()
{
	// SHADER DELETION
	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);
	glDeleteProgram(ShaderProgramID);
}
/*--------------------------- DEFAULT SHADERS --------------------------------*/
bool ShaderProgram::CreateDefaultVertexShader()
{
	std::cout << "/*--------------------------- DEFAULT SHADERS --------------------------------*/";
	Engine & engineRef = GetRenderer().GetEngine();
	/*--------------------------- VERTEX SHADER --------------------------------*/
	// Reads the content of vertex shader code into a string
	std::string vertexSource = std::string(engineRef.GetResourceManager().LoadTextFile("DefaultVertexShader.glsl", READ).pData);
	VertexShader = glCreateShader(GL_VERTEX_SHADER);	
	vertexFile[0] = { vertexSource.c_str() };
	glShaderSource(VertexShader, 1, vertexFile, NULL);	
	glCompileShader(VertexShader);
	// Debug vertex shader
	return CheckVertexShaderCompilationStatus(VertexShader);
}

bool ShaderProgram::CreateDefaultFragmentShader()
{
	Engine & engineRef = GetRenderer().GetEngine();
	/*--------------------------- FRAGMENT SHADER --------------------------------*/
    // Reads the content of fragment shader code into a string
	std::string fragmentSource = std::string(engineRef.GetResourceManager().LoadTextFile("DefaultFragmentShader.glsl" , READ).pData);	
	FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);	
	fragmentFile[0] = { fragmentSource.c_str() };		
	glShaderSource(FragmentShader, 1, fragmentFile, NULL);
	glCompileShader(FragmentShader);
	// Debug fragment shader
	return CheckFragmentShaderCompilationStatus(FragmentShader);
}
bool ShaderProgram::CreateDefaultGeometryShader()
{
	Engine & engineRef = GetRenderer().GetEngine();
	/*--------------------------- GEOMETRY SHADER --------------------------------*/
	// Reads the content of geometry shader code into a string
	std::string geometrySource = std::string(engineRef.GetResourceManager().LoadTextFile("DefaultGeometryShader.glsl", READ).pData);
	GeometryShader = glCreateShader(GL_GEOMETRY_SHADER);
	geometryFile[0] = { geometrySource.c_str() };
	glShaderSource(GeometryShader, 1, geometryFile, NULL);
	glCompileShader(GeometryShader);
	// Debug geometry shader
	return CheckGeometryShaderCompilationStatus(GeometryShader);
}
bool ShaderProgram::CreateDefaultShaderProgram()
{
	/*--------------------------- SHADER CREATION --------------------------------*/
	CreateDefaultVertexShader();
	CreateDefaultGeometryShader();
	CreateDefaultFragmentShader();
	ShaderProgramID = glCreateProgram();
	// SHADER ATTACHMENT
	glAttachShader(ShaderProgramID, VertexShader);
	glAttachShader(ShaderProgramID, GeometryShader);
	glAttachShader(ShaderProgramID, FragmentShader);
	//glBindFragDataLocation(shaderProgram, 0, "outColor"); // Not necessary, but when using multiple buffers, binds the fragment data output to the right buffer

	// LINKING
	glLinkProgram(ShaderProgramID);					
	// VALIDATION
	glValidateProgram(ShaderProgramID);
	// Debug program compilation status
	return CheckProgramCompilationStatus(ShaderProgramID);
}
/*--------------------------- DEBUG WIREFRAME & FACE, VERTEX NORMALS SHADERS ---------------------------------*/

/*--------------------------- SHADER CREATION - WIREFRAMES, NORMALS --------------------------------*/
bool ShaderProgram::CreateDebugNormalsShaderProgram()
{
	/*--------------------------- SHADER CREATION --------------------------------*/
	CreateDebugNormalsVertexShader();
	CreateDebugNormalsGeometryShader();
	CreateDebugNormalsFragmentShader();
	ShaderProgramID = glCreateProgram();
	// SHADER ATTACHMENT
	glAttachShader(ShaderProgramID, VertexShader);
	glAttachShader(ShaderProgramID, GeometryShader);
	glAttachShader(ShaderProgramID, FragmentShader);
	//glBindFragDataLocation(shaderProgram, 0, "outColor"); // Not necessary, but when using multiple buffers, binds the fragment data output to the right buffer

	// LINKING
	glLinkProgram(ShaderProgramID);
	// VALIDATION
	glValidateProgram(ShaderProgramID);
	// Debug program compilation status
	return CheckProgramCompilationStatus(ShaderProgramID);
}


bool ShaderProgram::CreateDebugNormalsVertexShader()
{
	std::cout << "/*--------------------------- DEBUG WIREFRAME & FACE, VERTEX NORMALS SHADERS --------------------------------*/";
	Engine & engineRef = GetRenderer().GetEngine();
	/*--------------------------- VERTEX SHADER --------------------------------*/
	// Reads the content of vertex shader code into a string
	std::string vertexSource = std::string(engineRef.GetResourceManager().LoadTextFile("NormalsVertexShader.glsl", READ).pData);
	VertexShader = glCreateShader(GL_VERTEX_SHADER);
	vertexFile[0] = { vertexSource.c_str() };
	glShaderSource(VertexShader, 1, vertexFile, NULL);
	glCompileShader(VertexShader);
	// Debug vertex shader
	return CheckVertexShaderCompilationStatus(VertexShader);
}

bool ShaderProgram::CreateDebugNormalsFragmentShader()
{
	Engine & engineRef = GetRenderer().GetEngine();
	/*--------------------------- FRAGMENT SHADER --------------------------------*/
	// Reads the content of fragment shader code into a string
	std::string fragmentSource = std::string(engineRef.GetResourceManager().LoadTextFile("NormalsFragmentShader.glsl", READ).pData);
	FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	fragmentFile[0] = { fragmentSource.c_str() };
	glShaderSource(FragmentShader, 1, fragmentFile, NULL);
	glCompileShader(FragmentShader);
	// Debug fragment shader
	return CheckFragmentShaderCompilationStatus(FragmentShader);
}

bool ShaderProgram::CreateDebugNormalsGeometryShader()
{
	Engine & engineRef = GetRenderer().GetEngine();
	/*--------------------------- GEOMETRY SHADER --------------------------------*/
	// Reads the content of geometry shader code into a string
	std::string geometrySource = std::string(engineRef.GetResourceManager().LoadTextFile("NormalsGeometryShader.glsl", READ).pData);
	GeometryShader = glCreateShader(GL_GEOMETRY_SHADER);
	geometryFile[0] = { geometrySource.c_str() };
	glShaderSource(GeometryShader, 1, geometryFile, NULL);
	glCompileShader(GeometryShader);
	// Debug geometry shader
	return CheckGeometryShaderCompilationStatus(GeometryShader);
}

/*--------------------------- DEBUG LINE SHADERS --------------------------------*/

/*--------------------------- SHADER CREATION - LINES --------------------------------*/
bool ShaderProgram::CreateDebugMeshShaderProgram()
{
	/*--------------------------- SHADER CREATION --------------------------------*/
	CreateDebugMeshVertexShader();
	CreateDebugMeshFragmentShader();
	ShaderProgramID = glCreateProgram();
	// SHADER ATTACHMENT
	glAttachShader(ShaderProgramID, VertexShader);
	glAttachShader(ShaderProgramID, FragmentShader);
	//glBindFragDataLocation(shaderProgram, 0, "outColor"); // Not necessary, but when using multiple buffers, binds the fragment data output to the right buffer

	// LINKING
	glLinkProgram(ShaderProgramID);
	// VALIDATION
	glValidateProgram(ShaderProgramID);
	// Debug program compilation status
	return CheckProgramCompilationStatus(ShaderProgramID);
}

bool ShaderProgram::CreateDebugMeshVertexShader()
{
	std::cout << "/*--------------------------- DEBUG LINE SHADERS --------------------------------*/";
	Engine & engineRef = GetRenderer().GetEngine();
	/*--------------------------- VERTEX SHADER --------------------------------*/
	// Reads the content of vertex shader code into a string
	std::string vertexSource = std::string(engineRef.GetResourceManager().LoadTextFile("LineVertexShader.glsl", READ).pData);
	VertexShader = glCreateShader(GL_VERTEX_SHADER);
	vertexFile[0] = { vertexSource.c_str() };
	glShaderSource(VertexShader, 1, vertexFile, NULL);
	glCompileShader(VertexShader);
	// Debug vertex shader
	return CheckVertexShaderCompilationStatus(VertexShader);
}

bool ShaderProgram::CreateDebugMeshFragmentShader()
{
	Engine & engineRef = GetRenderer().GetEngine();
	/*--------------------------- FRAGMENT SHADER --------------------------------*/
	// Reads the content of fragment shader code into a string
	std::string fragmentSource = std::string(engineRef.GetResourceManager().LoadTextFile("LineFragmentShader.glsl", READ).pData);
	FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	fragmentFile[0] = { fragmentSource.c_str() };
	glShaderSource(FragmentShader, 1, fragmentFile, NULL);
	glCompileShader(FragmentShader);
	// Debug fragment shader
	return CheckFragmentShaderCompilationStatus(FragmentShader);
}

/*--------------------------- BILLBOARDING QUAD SHADERS --------------------------------*/

/*--------------------------- SHADER CREATION - BILLBOARDING QUADS --------------------------------*/
bool ShaderProgram::CreateBillboardingQuadShaderProgram()
{
	/*--------------------------- SHADER CREATION --------------------------------*/
	CreateBillboardingQuadVertexShader();
	CreateBillboardingQuadFragmentShader();
	ShaderProgramID = glCreateProgram();
	// SHADER ATTACHMENT
	glAttachShader(ShaderProgramID, VertexShader);
	glAttachShader(ShaderProgramID, FragmentShader);
	//glBindFragDataLocation(shaderProgram, 0, "outColor"); // Not necessary, but when using multiple buffers, binds the fragment data output to the right buffer

	// LINKING
	glLinkProgram(ShaderProgramID);
	// VALIDATION
	glValidateProgram(ShaderProgramID);
	// Debug program compilation status
	return CheckProgramCompilationStatus(ShaderProgramID);
}

bool ShaderProgram::CreateBillboardingQuadVertexShader()
{
	std::cout << "/*--------------------------- BILLBOARDING QUAD SHADERS --------------------------------*/";
	Engine & engineRef = GetRenderer().GetEngine();
	/*--------------------------- VERTEX SHADER --------------------------------*/
	// Reads the content of vertex shader code into a string
	std::string vertexSource = std::string(engineRef.GetResourceManager().LoadTextFile("QuadVertexShader.glsl", READ).pData);
	VertexShader = glCreateShader(GL_VERTEX_SHADER);
	vertexFile[0] = { vertexSource.c_str() };
	glShaderSource(VertexShader, 1, vertexFile, NULL);
	glCompileShader(VertexShader);
	// Debug vertex shader
	return CheckVertexShaderCompilationStatus(VertexShader);
}
bool ShaderProgram::CreateBillboardingQuadFragmentShader()
{
	Engine & engineRef = GetRenderer().GetEngine();
	/*--------------------------- FRAGMENT SHADER --------------------------------*/
	// Reads the content of fragment shader code into a string
	std::string fragmentSource = std::string(engineRef.GetResourceManager().LoadTextFile("QuadFragmentShader.glsl", READ).pData);
	FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	fragmentFile[0] = { fragmentSource.c_str() };
	glShaderSource(FragmentShader, 1, fragmentFile, NULL);
	glCompileShader(FragmentShader);
	// Debug fragment shader
	return CheckFragmentShaderCompilationStatus(FragmentShader);
}


/*------------------------- SHADER AND PROGRAM COMPILATION LOGGING FUNCTIONS ------------------------- */
bool ShaderProgram::CheckVertexShaderCompilationStatus(GLuint aShaderID)
{
	/*--------------------------- VERTEX SHADER DEBUG --------------------------------*/
	GLint status;
	glGetShaderiv(aShaderID, GL_COMPILE_STATUS, &status);
	if (status == GL_TRUE)
	{
		std::cout << "\nVertex shader compiled successfully.";
		return true;
	}
	else
	{
		std::cout << "\nVertex shader failed to compile.";
		char buffer[512];
		glGetShaderInfoLog(aShaderID, 512, NULL, buffer);
		std::cout << buffer;
		return false;
	}
}

bool ShaderProgram::CheckFragmentShaderCompilationStatus(GLuint aShaderID)
{
	/*--------------------------- FRAGMENT SHADER DEBUG --------------------------------*/
	GLint status;
	glGetShaderiv(aShaderID, GL_COMPILE_STATUS, &status);
	if (status == GL_TRUE)
	{
		std::cout << "\nFragment shader compiled successfully.";
		return true;
	}
	else
	{
		std::cout << "\nFragment shader failed to compile.";
		char buffer[512];
		glGetShaderInfoLog(aShaderID, 512, NULL, buffer);
		std::cout << buffer;
		return false;
	}
}

bool ShaderProgram::CheckGeometryShaderCompilationStatus(GLuint aShaderID)
{
	/*--------------------------- GEOMETRY SHADER DEBUG --------------------------------*/
	GLint status;
	glGetShaderiv(GeometryShader, GL_COMPILE_STATUS, &status);
	if (status == GL_TRUE)
	{
		std::cout << "\nGeometry shader compiled successfully.";
		return true;
	}
	else
	{
		std::cout << "\nGeometry shader failed to compile.";
		char buffer[512];
		glGetShaderInfoLog(GeometryShader, 512, NULL, buffer);
		std::cout << buffer;
		return false;
	}
}

bool ShaderProgram::CheckProgramCompilationStatus(GLuint aProgramID)
{
	/*--------------------------- SHADER PROGRAM DEBUG --------------------------------*/
	GLint status;
	glGetProgramiv(aProgramID, GL_LINK_STATUS, &status);
	if (status == GL_TRUE)
	{
		std::cout << "\nShader program compiled successfully." << std::endl;
		return true;
	}
	else
	{
		std::cout << "\nShader program failed to compile.";
		char buffer[512];
		glGetShaderInfoLog(aProgramID, 512, NULL, buffer);
		std::cout << buffer << std::endl;
		return false;
	}
}
