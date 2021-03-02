#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 texCoord;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMat;

void main()
{
   //FragPos = vec3( view * model * vec4(aPos, 1.0)); //calculations in view space
   FragPos = vec3(model * vec4(aPos, 1.0)); //world space
   Normal = normalMat * aNormal; 
   texCoord = aTexCoord; 


	//gl_Position = projection * vec4(FragPos, 1.0f); //view space
	gl_Position = projection * view * vec4(FragPos, 1.0); //world space
}