#version 450 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 color;
layout(location = 3) in vec2 texcoord;

uniform mat4 ModelViewProjectionMatrix;
uniform mat4 Model;

out vec3 VertexPosition;
out vec3 VertexNormal;
out vec4 VertexColor;
out vec2 VertexUV;

void main()
{
	gl_Position = ModelViewProjectionMatrix * vec4(position.x, position.y, position.z, 1.0); // Vertex information so it is definitely a point hence, w can always be 1
	VertexPosition = vec3(Model * vec4(position, 1.0f));
	VertexColor = color;
	VertexNormal = normal;
	VertexUV = texcoord;
}