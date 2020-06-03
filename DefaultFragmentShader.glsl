#version 450 core

in vec3 GeometryPosition;
in vec3 GeometryNormal;
in vec4 GeometryColor;
in vec2 GeometryUV;

out vec4 Out_color;
uniform sampler2D Texture;
uniform vec3 LightColor;
uniform vec3 LightPosition;
uniform vec3 ViewPosition;

void main()
{
	/* -- Ambient color -- */
	float ambientStrength = 0.5f;
	vec3 ambientColor = ambientStrength * LightColor;

	/* -- Diffuse color -- */
	vec3 normal = normalize(GeometryNormal);
	// vector from fragment to light
	vec3 lightDirection = normalize(LightPosition - GeometryPosition);

	// Goes to 0 as angle between normal and light direction increases
	// i.e. if fragment is viewed straight on, has highest diffuse intensity
	float diffuse = max(dot(normal, lightDirection), 0.0f);
	vec3 diffuseColor = diffuse * LightColor;

	/* -- Specular color -- */
	float specularStrength = 0.5f;
	vec3 viewDirection = normalize(ViewPosition - GeometryPosition);
	vec3 reflectionDirection = reflect(-lightDirection, normal);

	// The higher the shininess value of an object, the more it properly reflects the light 
	// instead of scattering it all around and thus the smaller the highlight becomes
	float shininess = 32;

	float specular = pow(max(dot(viewDirection, reflectionDirection), 0.0f), shininess);
	vec3 specularColor = specularStrength * specular * LightColor;

	/* -- Final color -- */
	vec3 objectColor = vec3(GeometryColor.x * LightColor.x, GeometryColor.y * LightColor.y, GeometryColor.z * LightColor.z);
	vec3 result = (ambientColor + diffuseColor + specularColor) * objectColor;
	Out_color = vec4(result, 1.0);
}