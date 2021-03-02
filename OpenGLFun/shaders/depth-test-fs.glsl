#version 330 core 
out vec4 FragColor;

in vec2 TexCoord;
//in vec3 Normal;
//in vec3 FragPos;

uniform sampler2D texture1;

float near = 0.1;
float far = 100.0;

float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0; //back to NDC
	return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
	vec4 texColor = texture(texture1, TexCoord);
	
	FragColor = texColor;
}