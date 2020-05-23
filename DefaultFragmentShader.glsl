#version 450 core
in vec3 GeometryNormal;
in vec4 GeometryColor;
in vec2 GeometryUV;

out vec4 out_color;
uniform sampler2D Texture;
void main()
{
	out_color = /*texture(Texture, GeometryUV) **/ vec4(GeometryColor.x, GeometryColor.y, GeometryColor.z, 1.0);
}