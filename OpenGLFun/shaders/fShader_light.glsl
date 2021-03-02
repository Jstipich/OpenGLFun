#version 330 core
struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D texture3;
uniform sampler2D texture4;


void main()
{
	FragColor = vec4(1.0);// * texture(texture3, texCoord); //set all four values to 1.0
}