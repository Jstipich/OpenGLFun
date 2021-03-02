#include <glad/glad.h>
#include <glfw3.h>
#include <iostream>
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <math.h>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(char const* path, bool transparent);
void setLightPerams(Shader temp, glm::vec3* ptLightPos);

//settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//timing
float deltaTime = 0.0f;// time between current frsme and last frame
float lastFrame = 0.0f;// time of last frame

//camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0f;
float lastY = (float)SCR_HEIGHT / 2.0f;
bool firstMouse = true;

int main()
{
	//glfw initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //would be used if I was using a mac

	//glfw window creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGLFun", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	

	//load all of the openGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//configure global opengl state 
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);// stencil test fails, stencil test passes but depth test fails, both pass
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //allways pass the depth test (same effect as glDisable(GL_DEPTH_TEST))

	stbi_set_flip_vertically_on_load(true);

	//build and compile our shader program
	//Shader cubeShader("shaders/vShader.glsl", "shaders/fShader.glsl");
	//Shader lightShader("shaders/vShader_light.glsl", "shaders/fShader_light.glsl");
	//Shader backPackShader("shaders/modelvs.glsl", "shaders/modelfs.glsl");
	Shader cubeShader("shaders/depth-test-vs.glsl", "shaders/depth-test-fs.glsl"); //we're just doing this for this chapter
	//Shader ShaderSingleColor("shaders/stencil_test_vs.glsl", "shaders/stencil_test_fs.glsl");

	float vertices[] = {
		// back face
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right    
		 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right              
		 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
		-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // top-left
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left                
		// front face
		-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
		 0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
		 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // bottom-right        
		 0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
		-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
		-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // top-left        
		// left face
		-0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
		-0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left       
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
		-0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
		-0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-right
		// right face
		 0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
		 0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right      
		 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right          
		 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
		 0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
		 0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
		// bottom face          
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
		 0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
		 0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left        
		 0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
		-0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
		// top face
		-0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
		 0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right
		 0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right                 
		 0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
		-0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // bottom-left  
		-0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top-left              
	};

	float planeVertices[] = {
		//positions           //normals          //tex coords
		 5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f,  2.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f,  0.0f, 2.0f,

		 5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f,  2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f,  0.0f, 2.0f,
		 5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f,  2.0f, 2.0f
	};

	float grassVertices[] = {
		//positions          //normals          //tex coords
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,

		 0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f

	};

	float quadVertices[] = {
		// positions   // texCoords
	   -1.0f,  1.0f,  0.0f, 1.0f,
	   -1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

	   -1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};

	/*
	// positions of cubes in world space
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f, 0.2f, 2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f, 2.0f, -12.0f),
		glm::vec3(0.0f, 0.0f, -3.0f)
	};
	*/

	vector<glm::vec3> vegitation
	{
		glm::vec3(-1.5f, 0.0f, -0.48f),
		glm::vec3( 1.5f, 0.0f,  0.51f),
		glm::vec3( 0.0f, 0.0f,  0.7f),
		glm::vec3(-0.3f, 0.0f, -2.3f),
		glm::vec3( 0.5f, 0.0f, -0.6f)
	};

	//configure the cube's VAO and VBO
	//--------------------------------
	unsigned int cubeVBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);

	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);
	//position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//for the normal vectors
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// for textures
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	//second, configure the plane VAO and VBO
	//---------------------------------------
	unsigned int planeVBO, planeVAO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);

	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

	glBindVertexArray(planeVAO);
	//position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//textures
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	unsigned int grassVBO, grassVAO;
	glGenVertexArrays(1, &grassVAO);
	glGenBuffers(1, &grassVBO);

	glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grassVertices), grassVertices, GL_STATIC_DRAW);

	glBindVertexArray(grassVAO);
	//position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//textures
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	unsigned int quadVBO, quadVAO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	//load and create a texture
	//-------------------------
	unsigned int cubeTexture = loadTexture("textures/marble-texture.jpg", false);
	unsigned int floorTexture = loadTexture("textures/test texture.jpg", false);
	unsigned int bTexture = loadTexture("textures/blending_transparent_window.png", true);

	cubeShader.use();
//	cubeShader.setInt("material.diffuse", 4);
//	cubeShader.setInt("material.specular", 5);
//	cubeShader.setInt("material.emission", 6);
//	cubeShader.setFloat("material.shininess", 64.0f);
	
	cubeShader.setInt("texture1", 0);

	//Model ourModel("models/backpack/backpack.obj");
	//Model testModel("models/adjusted hallway/adjusted.obj");

	//configure framebuffer
	//---------------------
	unsigned int frameBuffer;
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	//generate texture 
	unsigned int texColorBuffer;
	glGenTextures(1, &texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	//attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	//renderbuffer object for depth and stencil testing
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	//draw as wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//render loop
	//-----------
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//input
		processInput(window);

		//rendering goes here
//		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glStencilFunc(GL_ALWAYS, 1, 0xFF); //all fragments should pass the stencil test 

		//setLightPerams(cubeShader, pointLightPositions);
		//setLightPerams(backPackShader, pointLightPositions);

		cubeShader.use();
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);//create the projection matrix (moved into the render loop since we have zoom implemented)
		cubeShader.setMat4("view", view);
		cubeShader.setMat4("projection", projection);

		std::map<float, glm::vec3> sorted;//sorting the semi-transparent images so it draws correctly
		for (unsigned int i = 0; i < vegitation.size(); i++) {
			float distance = glm::length(camera.Position - vegitation[i]);
			sorted[distance] = vegitation[i];
		}

		//floor
	
		glBindVertexArray(planeVAO);
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		cubeShader.setMat4("model", glm::mat4(1.0f));
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		//cubes
		glBindVertexArray(cubeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);
		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
		//glm::mat3 normalMat(glm::transpose(glm::inverse(model)));
		cubeShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
		cubeShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// go and re-read stencil testing before you try to impliment it again (I'm gonna not do much with it in the forseeable future)
		//grass!
		glDisable(GL_CULL_FACE);
		glBindVertexArray(grassVAO);
		glBindTexture(GL_TEXTURE_2D, bTexture);
		for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); it++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, it->second);
			cubeShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		glEnable(GL_CULL_FACE);

		//check and call the events and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &cubeVBO);
	glDeleteBuffers(1, &planeVBO);

	glfwTerminate();
	return 0;

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FOREWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
}

//whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; //reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

//whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const* path, bool transparent)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		if (transparent)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void setLightPerams(Shader temp,glm::vec3 *ptLightPos)
{
	temp.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
	temp.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
	temp.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
	temp.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

	temp.setVec3("pointLights[0].position", ptLightPos[0]);
	temp.setFloat("pointLights[0].constant", 1.0f);
	temp.setFloat("pointLights[0].linear", 0.09f);
	temp.setFloat("pointLights[0].quadratic", 0.032f);
	temp.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
	temp.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
	temp.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);

	temp.setVec3("pointLights[1].position", ptLightPos[1]);
	temp.setFloat("pointLights[1].constant", 1.0f);
	temp.setFloat("pointLights[1].linear", 0.09f);
	temp.setFloat("pointLights[1].quadratic", 0.032f);
	temp.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
	temp.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
	temp.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);

	temp.setVec3("pointLights[2].position", ptLightPos[2]);
	temp.setFloat("pointLights[2].constant", 1.0f);
	temp.setFloat("pointLights[2].linear", 0.09f);
	temp.setFloat("pointLights[2].quadratic", 0.032f);
	temp.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
	temp.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
	temp.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);

	temp.setVec3("pointLights[3].position", ptLightPos[3]);
	temp.setFloat("pointLights[3].constant", 1.0f);
	temp.setFloat("pointLights[3].linear", 0.09f);
	temp.setFloat("pointLights[3].quadratic", 0.032f);
	temp.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
	temp.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
	temp.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);

	temp.setVec3("spotLight.position", camera.Position);
	temp.setVec3("spotLight.direction", camera.Front);
	temp.setFloat("spotLight.constant", 1.0f);
	temp.setFloat("spotLight.linear", 0.09f);
	temp.setFloat("spotLight.quadratic", 0.032f);
	temp.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	temp.setVec3("spotLight.diffuse", 0.5f, 0.5f, 0.5f);
	temp.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
	temp.setFloat("spotLight.cutOff", glm::cos(glm::radians(14.2f)));
	temp.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(18.2)));

	temp.setFloat("material.shininess", 64.0f);
}