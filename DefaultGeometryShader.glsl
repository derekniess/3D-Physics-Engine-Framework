#version 330 compatibility
layout(triangles) in;
layout(triangle_strip, max_vertices=4) out;

in vec3 VertexNormal[];
in vec4 VertexColor[];
in vec2 VertexUV[];

out vec3 GeometryNormal;
out vec4 GeometryColor;
out vec2 GeometryUV;

void main()
{

  for(int i=0; i < gl_in.length(); i++)
  {
    gl_Position = gl_in[i].gl_Position;
	GeometryColor = VertexColor[i].xyzw;
	GeometryUV = VertexUV[i].xy;
    EmitVertex();
  }

  EndPrimitive();
}  