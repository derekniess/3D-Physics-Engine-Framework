#version 450 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;

uniform mat4 ModelViewProjectionMatrix;

out vec4 VertexColor;
void main()
{
	gl_Position = ModelViewProjectionMatrix * vec4(position.x, position.y, position.z, 1.0); // Vertex information so it is definitely a point hence, w can always be 1
	VertexColor = color;
}