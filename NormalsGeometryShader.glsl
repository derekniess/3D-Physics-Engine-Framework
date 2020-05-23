#version 330 compatibility
layout(triangles) in;
layout(line_strip, max_vertices=8) out;

uniform mat4 ModelViewProjectionMatrix;

in vec4 VertexColor[];
in vec3 VertexNormal[];
in vec2 VertexUV[];

out vec4 GeometryColor;
out vec2 GeometryUV;

void main()
{	
  GeometryColor = VertexColor[0].xyzw;
  GeometryUV = VertexUV[0].xy;

    // Draw vertex normal
  for(int i=0; i<gl_in.length(); i++)
  {
    vec3 P = gl_in[i].gl_Position.xyz;
    vec3 N = VertexNormal[0].xyz;
    
    gl_Position = ModelViewProjectionMatrix * vec4(P, 1.0);
	GeometryColor = vec4(1,0,0,1);
    EmitVertex();
    
    gl_Position = ModelViewProjectionMatrix * vec4(P + N * 1 , 1.0);
    GeometryColor = vec4(1,0,0,1);
    EmitVertex();
    
    EndPrimitive();
  }

  // Draw face normal
  vec3 P0 = gl_in[0].gl_Position.xyz;
  vec3 P1 = gl_in[1].gl_Position.xyz;
  vec3 P2 = gl_in[2].gl_Position.xyz;
  
  vec3 V0 = P0 - P1;
  vec3 V1 = P2 - P1;
  
  vec3 N = cross(V1, V0);
  N = normalize(N);
  
  // Center of the triangle
  vec3 P = (P0+P1+P2) / 3.0;
  
  gl_Position = ModelViewProjectionMatrix * vec4(P, 1.0);
  GeometryColor = vec4(1, 1, 0, 1);
  EmitVertex();
  
  gl_Position = ModelViewProjectionMatrix * vec4(P + N * 1, 1.0);
  GeometryColor = vec4(1, 1, 0, 1);
  EmitVertex();

  EndPrimitive();

}  