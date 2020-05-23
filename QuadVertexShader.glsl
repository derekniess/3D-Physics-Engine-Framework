#version 450 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
out vec4 VertexColor;

uniform int BillboardMode; // 1 for spherical, 0 for cylindrical

void main()
{
	mat4 modelView = ViewMatrix * ModelMatrix;
	vec3 scale;
	scale.x = ModelMatrix[0][0];
	scale.y = ModelMatrix[1][1];
	scale.z = ModelMatrix[2][2];
	
	// First colunm.
	modelView[0][0] = scale.x; 
	modelView[0][1] = 0.0; 
	modelView[0][2] = 0.0; 

	if (BillboardMode == 1)
	{
	  // Second colunm.
	  modelView[1][0] = 0.0; 
	  modelView[1][1] = scale.y; 
	  modelView[1][2] = 0.0; 
	}

	// Thrid colunm.
	modelView[2][0] = 0.0; 
	modelView[2][1] = 0.0; 
	modelView[2][2] = scale.z; 
	
	mat4 modelViewProjectionMatrix = ProjectionMatrix * modelView;

	gl_Position = modelViewProjectionMatrix * vec4(position.x, position.y, position.z, 1.0); // Vertex information so it is definitely a point hence, w can always be 1
	VertexColor = color;
}