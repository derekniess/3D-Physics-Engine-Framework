#version 450 core
in vec4 VertexColor;

out vec4 out_color;
void main()
{
	out_color = vec4(VertexColor.x, VertexColor.y, VertexColor.z, 1.0);
}