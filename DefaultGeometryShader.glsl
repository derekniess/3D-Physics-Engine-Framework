#version 330 compatibility
layout(triangles) in;
layout(triangle_strip, max_vertices=4) out;

in vec3 VertexPosition[];
in vec3 VertexNormal[];
in vec4 VertexColor[];
in vec2 VertexUV[];

out vec3 GeometryPosition;
out vec3 GeometryNormal;
out vec4 GeometryColor;
out vec2 GeometryUV;

void main()
{

	for(int i=0; i < gl_in.length(); i++)
	{
		gl_Position = gl_in[i].gl_Position;
		GeometryPosition = VertexPosition[i].xyz;
		GeometryNormal = VertexNormal[i].xyz;
		GeometryColor = VertexColor[i].xyzw;
		GeometryUV = VertexUV[i].xy;
		EmitVertex();
	}

	EndPrimitive();
}  