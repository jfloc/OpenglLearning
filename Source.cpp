#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>

bool isFileExists(const std::string& filename) {
	std::ifstream file(filename);
	return file.good();
}

#include <Shader.h>
#include <camera.h>
#include "PoissonDiskSampling3D.h"
#include <cmath>


#include "CreateShape.h"


// camera

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void createPolygonFace(std::vector<float>& vertices, int numVertices);
void createElementDrawingIndices(unsigned int*& indices, int numVertices, const int indiceSize);
void createShapeBuffer(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, std::vector<float> vertices, int verticeSize, unsigned int* indices, int indiceSize);
void createGLTriangleIndices(unsigned int*& index, int numVertices);
std::vector<float> getSurfaceNormal(std::vector<float> polygon);
std::vector<float> getAllNormals(std::vector<float> normal, unsigned int* index, int numSize);
std::vector<glm::vec3> getCubePositions(int numCountModels);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.5f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = (SCR_HEIGHT / 2.0f);
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


int main()
{

	/*const char *filepath = "leaves.png";
	if (isFileExists(filepath)) {
		std::cout << "Image file exists and can be opened." << std::endl;
	}
	else {
		std::cout << "Failed to open the image file." << std::endl;
	}*/

	glm::mat4 trans = glm::mat4(1.0f);
	//trans = glm::rotate(trans,glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
#endif

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "GLFW window failed to create." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to load GLAD." << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	Shader ourShader("3.3.shader.vs", "3.3.shader.fs");
	Shader lightingShader("3.3.shader.vs", "lightingShader.fs");
	/*
	* Everything under this creates the shapes.
	*/

	const int numVertices = 18; // Correct number of vertices for the cone

	//char dir[] = "plan";
	const int verticeSize = ((numVertices + 3) * 12); // creates the size of vertices needed for a vector of numVertices plus 1 for top and 1 for dupe vertice with different texture mapping coordinates to avoid texture bugs on a face, times 9 because of vertice pattern (x,y,z,r,g,b,a,xTexCoord,yTexCoord, )

	std::vector<float> coneFacesVertices; // first vector holding vertices of circle to the top point
	createPolygonFace(coneFacesVertices, numVertices); // updates vector<float> vertex to have the vertices

	std::cout << "size of cone vertices is " << coneFacesVertices.size() << std::endl;
	// second vertex creation but for the bottom of the tree to render the base of the cone.
	/*std::vector<float> coneBaseVertices;
	char dir2[] = "plane";
	createPolygonFace(coneBaseVertices, numVertices, dir2);

	coneFacesVertices.insert(coneFacesVertices.end(), coneBaseVertices.begin(), coneBaseVertices.begin()+9);*/

	// creation of indices in order from Center vertex, 0 , 1, 1, 2, ... , n -1 , n , where n is the number of vertices holding same vertex data (x,y,z) and different texture coords. 
	int i, j;

	const int indiceSize = ((numVertices * 6));
	unsigned int* index;
	createGLTriangleIndices(index, numVertices);

	unsigned int indices[numVertices * 6];
	for (i = 0; i < numVertices * 6; i++)
	{
		//std::cout << "\n index surace normal "index[i];
		indices[i] = index[i];
		std::cout << indices[i];
	}
	// copying to indices array getting data from above starting at the pointer pointed by index[0] to the last indice.
	std::cout << "\nthis is the size of index " << std::endl;

	std::vector<float> allNormals = getAllNormals(coneFacesVertices, indices, numVertices);
	std::cout << "\n{";
	/*
	for (i = 0; i < allNormals.size(); i += 12) {
		//allNormals.at(i)
		for (j = 0; j < 12; j++) {
			std::cout << allNormals.at(j + i);
			if (j != 11) std::cout << ", ";
			else std::cout << std::endl;

		}
		//std::cout << allNormals.at(i) << ", " <<  allNormals.at(i + 1) << ", " << allNormals.at(i + 2) <<", " << std::endl;
	}
	*/
	std::cout << "}\n";


	std::cout << coneFacesVertices.size();
	/*for (i = 0; i < coneFacesVertices.size(); i+=1) {
		std::cout << "Vertex at " << i/9 << ":";
		for (j = i; j < i + 9; j++) {
			std::cout << coneFacesVertices.at(j) << " ";
		}

		std::cout << std::endl;
	}*/
	std::cout << std::endl;
	std::cout << "Num vertices in total is " << sizeof(coneFacesVertices) / sizeof(coneFacesVertices[0]) << std::endl;

	//float cubePos[((3) * 3)+1];
	/*int min = 0;
	int max = 6;
	std::vector<int> xPositions;
	for (i = 0; i < 9; i++) {
		xPositions.push_back(i);
	}
	for (i = 0; i < 9; i+=3) {
		cubePos[i] = rand()%(max-min + 1) +min;
		cubePos[i + 1] = 0.0f;
		cubePos[i + 2] = rand() % (2 - min + 1) + min;
		min += 1;
		if (cubePos[i] >= max)
	}*/

	/*for (i = 0; i < 9; i ++) {
		std::cout << "cubePos[" <<i<<"] : " << cubePos[i] << "\n";
	}*/

	//std::vector <float> cubeVertices = {
	//	// (x, y, z)		(r,		g,	  b,	a)		(xcoord , ycoord) for textures
	//	0.5f, 0.0f, 0.5f,	1.0f, 1.0f, 1.0f, 1.0f,		0.0f, 0.0f, // 0
	//   -0.5f, 0.0f, 0.5f,	1.0f, 1.0f, 1.0f, 1.0f,	   0.0f, 0.0f, // 1
	//   -0.5f, 0.0f,-0.5f,	1.0f, 1.0f, 1.0f, 1.0f,	   0.0f,0.0f, // 2
	//	0.5f, 0.0f,-0.5f,	1.0f, 1.0f, 1.0f, 1.0f,		0.0f,0.0f, // 3
	//	0.5f, 1.0f, 0.5f,	1.0f, 1.0f, 1.0f, 1.0f,		0.0f, 0.0f, // 4
	//   -0.5f, 1.0f, 0.5f,	1.0f, 1.0f, 1.0f, 1.0f,	   0.0f, 0.0f, // 5
	//   -0.5f, 1.0f,-0.5f,	1.0f, 1.0f, 1.0f, 1.0f,	   0.0f,0.0f, // 6
	//	0.5f, 1.0f,-0.5f,	1.0f, 1.0f, 1.0f, 1.0f,		0.0f,0.0f  // 7

	//};

	std::vector <float> cubeVertices = {
		-0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	-0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	-0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

	// Back face
	-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
	0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
	0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
	0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
	-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
	// Left face
   -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
   -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
   -0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
   -0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
   -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
   -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

   // Right face
   0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
   0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
   0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
   0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
   0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
   0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

   // Top face
   -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
   0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
   0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
   0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
   -0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
   -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

   // Bottom face
   -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
   0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
   0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
   0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
   -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
   -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f
	};



	unsigned int cubeIndices[] = {
		0,1,2,
		2,3,0,
		1,2,6,
		6,5,1,
		6,2,3,
		6,7,3,
		7,3,4,
		4,0,3,
		4,0,1,
		4,5,1,
		6,5,4,
		6,7,4
	};
	int size = (sizeof(cubeIndices) / sizeof(cubeIndices[0]));
	std::cout << "\n\n\n" << std::endl;

	//change this call to getCubePositions parameter to increase or decrease the number of tree models.
	std::vector<glm::vec3> cubeVectorPositions = getCubePositions(50);


	for (i = 0; i < cubeVectorPositions.size(); i++) {
		for (j = 0; j < cubeVectorPositions[i].length(); j++) {
			std::cout << cubeVectorPositions[i][j] << " ";

		}
		std::cout << typeid(cubeVectorPositions).name();
		std::cout << std::endl;
	}
	glm::vec3 cubePositions[] = {
		glm::vec3(1.0f,  0.0f,  0.0f),
		glm::vec3(2.5f,  0.0f, 0.0f),
		glm::vec3(4.0f,  0.0f, 0.0f),
		glm::vec3(5.5f,  0.0f, 0.0f),
		glm::vec3(7.0f,  0.0f, 0.0f),
		glm::vec3(8.5f,  0.0f, 0.0f),
	};
	unsigned int EBO, VBO, VAO, VAO2, VAO3, cube;

	glGenVertexArrays(1, &VAO);
	//glGenVertexArrays(1, &VAO2);
	glGenVertexArrays(1, &VAO3);
	glGenVertexArrays(1, &cube);

	createShapeBuffer(VAO, VBO, EBO, allNormals, allNormals.size(), indices, indiceSize);
	//createShapeBuffer(VAO2, VBO, EBO, coneBaseVertices, verticeSize, indices, indiceSize);
	//createShapeBuffer(VAO3, VBO, EBO, cubeVertices, cubeVertices.size(), cubeIndices, size);
	createShapeBuffer(cube, VBO, EBO, cubeVertices, cubeVertices.size(), cubeIndices, size);
	createShapeBuffer(VAO3, VBO, EBO, cubeVertices, cubeVertices.size(), cubeIndices, size);

	unsigned int texture;
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



	int width, height, nrChannels;

	unsigned char* data = stbi_load("leaves_rotated.png", &width, &height, &nrChannels, 0);

	if (data)
	{

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << stbi_failure_reason() << std::endl;
	}


	stbi_image_free(data);

	ourShader.use();
	ourShader.setInt("texture1", 0);
	//ourShader.setVec3("lightColor", glm::vec3(0.4f, 0.69f, 0.165f));

	lightingShader.use();
	/*CreateShape shape;
	shape.displayInfo();
	std::vector<float> x = (shape.createVertices(shape.getName()));
	std::vector<float> y = { 5,6,7,8 };
	x.insert(x.end(), y.begin(), y.end());*/

	std::cout << std::endl;
	/*for (i = 0; i < x.size(); i++) {
		std::cout << x.at(i) << " ";
	}*/
	std::cout << std::endl;
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		ourShader.use();

		ourShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
		ourShader.setVec3("lightPos", glm::vec3(1.0f, 1.0f, -2.0f));
		ourShader.setVec3("objectColor", glm::vec3(0.337f, 0.85f, 0.357f));
		ourShader.setVec3("viewPos", camera.Position);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), float(SCR_WIDTH) / float(SCR_HEIGHT), 0.1f, 100.0f);
		ourShader.setMat4("projection", projection);

		ourShader.setVec3("viewPos", camera.Position);
		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("view", view);


		glBindVertexArray(VAO);

		// Render the first shape
		for (unsigned int i = 0; i < cubeVectorPositions.size(); i++)
		{
			//std::cout << "size of x is " << cubeVectorPositions.size();
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubeVectorPositions[i]);
			ourShader.setMat4("model", model);



			//gldrawelements(gl_triangle_fan, sizetodraw, gl_unsigned_int, 0);
			glDrawArrays(GL_TRIANGLES, 0, allNormals.size());
		}
		


		/*for (unsigned int i = 0; i < (sizeof(cubePositions) / sizeof(cubePositions[0])); i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			ourShader.setMat4("model", model);

			glDrawElements(GL_TRIANGLE_FAN, indiceSize, GL_UNSIGNED_INT, 0);
		}*/

		// Render cone base
		/*for (unsigned int i = 0; i < (sizeof(cubePositions) / sizeof(cubePositions[0])); i++)
		{

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			ourShader.setMat4("model", model);

			glDrawElements(GL_TRIANGLE_FAN, indiceSize, GL_UNSIGNED_INT, 0);
		}*/

		// Assuming indices is the array containing indices for both cone faces and base





		// Draw the triangle
		// Update model matrix, shader uniforms, etc. if needed
		//glBindVertexArray(VAO);
		////glBindVertexArray(VAO2);
		//for (unsigned int i = 0; i < (sizeof(cubePositions) / sizeof(cubePositions[0])); i++)
		//{
		//	glm::mat4 model = glm::mat4(1.0f);
		//	model = glm::translate(model, cubePositions[i]);
		//	// You might need to apply different transformations here for the second shape
		//	ourShader.setMat4("model", model);

		//	glDrawElements(GL_TRIANGLE_FAN, indiceSize, GL_UNSIGNED_INT, 0);
		//}

		glBindVertexArray(cube);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0f, 0.5f, 3.0f));
		model = glm::scale(model, glm::vec3(0.5));
		ourShader.setMat4("model", model);
		ourShader.setVec3("objectColor", glm::vec3(1.0f, 0.51f, 0.165f));


		glDrawArrays(GL_TRIANGLES, 0, cubeVertices.size());

		glBindVertexArray(VAO3);
		lightingShader.use();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);


		//glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 2.0f));
		model = glm::scale(model, glm::vec3(0.1));
		// You might need to apply different transformations here for the second shape

		lightingShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, size, cubeVertices.size());;


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}


std::vector<glm::vec3> getCubePositions(int numCountModels) {
	glm::vec3 areaMin(0.0f, 0.0f, 0.0f);
	glm::vec3 areaMax(10.0f, 10.0f, 10.0f);
	float minDistance = 1.0f;
	int numPoints = numCountModels;
	
	PoissonDiskSampling3D sampler(areaMin, areaMax, minDistance, numPoints);
	std::vector<glm::vec3> points; 
	sampler.generatePoints();
	points = sampler.getPoints();

	return points;
}

void deleteVec3Array(glm::vec3* array) {
	delete[] array;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = static_cast<float>(2.5 * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{

	glViewport(0, 0, width, height);
}
/*
* The createPolygonFace takes the vertices array reference to update the array,
* the number of vertices needed to create the pyramid shape with a base of (n) vertices,
* and direction as a pointer to a char meaning whether we create the base or faces.
*/
void createPolygonFace(std::vector<float>& vertices, int numVertices)
{
	//vertices = new float[((numVertices+2) * 9)]; // 8 elements per vertex

	// Calculate the angle between two vertices
	float angleStep = 360.0f / numVertices;



	for (int i = 0; i < numVertices + 1; i++)
	{
		float angle = (i * angleStep) * (3.14159265359f) / (180.0f);
		float x = cos(angle);

		float y = sin(angle);
		vertices.push_back(0.5f * x);
		if (i == 5) {
			std::cout << "\n x is" << x << std::endl;

		}

		vertices.push_back(0.0f);
		vertices.push_back(0.5f * y);
		vertices.push_back(1.0f);
		vertices.push_back(1.0f);
		vertices.push_back(1.0f);
		vertices.push_back(1.0f);

		if (i % 2 == 0) {
			vertices.push_back(0.325f);
			vertices.push_back(0.2f);
		}
		else {
			vertices.push_back(0.675f);
			vertices.push_back(0.2f);
		}
	}


	for (int i = 0; i < 2; i++) {
		vertices.push_back(0.0f);
		if (i == 0) {
			vertices.push_back(0.0f);
		}
		else {
			vertices.push_back(2.0f);
		}

		vertices.push_back(0.0);
		vertices.push_back(1.0f);
		vertices.push_back(1.0f);
		vertices.push_back(1.0f);
		vertices.push_back(1.0f);
		vertices.push_back(0.5f);
		vertices.push_back(0.75f);
	}
	for (int i = 0; i < numVertices + 2; i++)
	{
		std::cout << "Vertex " << i << ": "
			<< vertices.at(i * 9) << ", " << vertices.at(i * 9 + 1) << ", " << vertices.at(i * 9 + 2) << std::endl;
	}
}

void createShapeBuffer(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, std::vector<float> vertices, int verticeSize, unsigned int* indices, int indiceSize) {

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, (verticeSize) * sizeof(float), vertices.data(), GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indiceSize * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(4 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(7 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(3);



}

// this function takes in triangleIndices all of them as a list first half a 
void createGLTriangleIndices(unsigned int*& triangleIndices, int numVertices) {

	int centerIndexIncrement = 1, track = 0;
	const int sizeofPartials = numVertices + 1;
	std::vector<unsigned int> indicePartials(sizeofPartials, 1);
	triangleIndices = new unsigned int[numVertices * 6];
	for (int i = 0; i < sizeofPartials; i++) {
		indicePartials.at(i) = i;
	}
	for (int i = 1; i < numVertices * 6; i += 3) {
		//Signifies the middle point of the indice array switching to using the last index as the first number of each triangle.
		if (i - 1 == numVertices * 3) {
			centerIndexIncrement = 2;
			track = 0;
		}
		triangleIndices[i - 1] = numVertices + centerIndexIncrement;
		triangleIndices[i] = indicePartials.at(track);
		triangleIndices[i + 1] = indicePartials.at(track + 1);
		track++;
	}

}
// This function creates the pattern for the indices using drawElment passing the indices which are in order {topIndex=n+1, 0, 1,1,2,2,...,n-1,n-1,n} where n is the number of vertices we want for our shape
void createElementDrawingIndices(unsigned int*& indices, int numVertices, const int indiceSize)
{
	if (indiceSize > 6) {
		indices = new unsigned int[indiceSize];
		indices[0] = numVertices + 1;
		indices[1] = 0;

		int i;

		for (i = 2; i <= (numVertices - 1) * 2; i += 2)
		{
			indices[i] = i / 2;
			indices[i + 1] = i / 2;
		}
		indices[numVertices * 2] = numVertices;
		//--------------------------------
		//| i |  indice[i] | indice[i+1] |
		//|---|------------|-------------|	
		//|	2 |		1	   |	 1		 |
		//|	4 |		2      |	 2		 |
		//|	6 |		3	   |	 3		 |
		//|___|____________|_____________| 
		//indices[numVertices * 2 + (1)] = numVertices + 2;

		for (i = 0; i < indiceSize; i++)
		{
			std::cout <<
				indices[i] << ", ";
		}
		std::cout << "\n" << std::endl;
	}
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

std::vector<float> getAllNormals(std::vector<float> polygon, unsigned int* index, int numSize)
{
	std::cout << polygon.size();
	std::cout << "entering polygon in get AllNormals \n" << std::endl;
	for (int i = 0; i < polygon.size(); i++) {
		std::cout << polygon.at(i) << " ";

	}
	std::vector<unsigned int> indices;
	for (int i = 0; i < numSize * 6; i += 3) {
		indices.push_back(index[i]);
		indices.push_back(index[i + 1]);
		indices.push_back(index[i + 2]);
	}

	std::vector<float> normals;
	for (int i = 0; i < indices.size(); i += 3) {
		std::vector<float> vec1(polygon.begin() + (indices.at(i) * 9), polygon.begin() + ((indices.at(i) * 9) + 9));
		std::vector<float> vec2(polygon.begin() + (indices.at(i + 1) * 9), polygon.begin() + ((indices.at(i + 1) * 9) + 9));
		std::vector<float> vec3(polygon.begin() + (indices.at(i + 2) * 9), polygon.begin() + ((indices.at(i + 2) * 9) + 9));

		std::vector<float> combinedVector;

		combinedVector.insert(combinedVector.end(), vec1.begin(), vec1.end());
		combinedVector.insert(combinedVector.end(), vec2.begin(), vec2.end());
		combinedVector.insert(combinedVector.end(), vec3.begin(), vec3.end());

		combinedVector = getSurfaceNormal(combinedVector);
		std::cout << std::endl;
		for (int i = 0; i < 3; i++) {
			std::vector<float> selectedVector;
			switch (i) {
			case 0:
				selectedVector = vec1;
				break;
			case 1:
				selectedVector = vec2;
				//vec2.insert(vec2.end(), combinedVector.begin(), combinedVector.end());
				break;
			case 2:
				selectedVector = vec3;
				//vec3.insert(vec3.end(), combinedVector.begin(), combinedVector.end());
				break;
			}
			selectedVector.insert(selectedVector.end(), combinedVector.begin(), combinedVector.end());
			normals.insert(normals.end(), selectedVector.begin(), selectedVector.end());
		}

		std::cout << std::endl;
		for (int j = 0; j < normals.size(); j++) {
			std::cout << " " << normals
				.at(j) << " ";
			if (j % 12 == 0 && j != 0) std::cout << std::endl;

		}
		std::cout << std::endl;
		/*
		std::cout << std::endl;
		std::cout << vec1.data() << std::endl;
		for (int j = 0; j < vec2.size(); j++) {
			std::cout << "vec2:" << vec2.at(j) << " ";
		}
		std::cout << std::endl;
		std::cout << vec2.data() << std::endl;
		for (int j = 0; j < vec3.size(); j++) {
			std::cout << "vec3:" << vec3.at(j) << " ";
		}*/
		/*std::cout << std::endl;
		std::cout << vec3.data() << std::endl;*/
	}

	//for (int i = 0; i < numSize * 2; i++) {
	//	//std::vector<float> vec1(polygon.begin() + (polygon.at(indices[0] * 9) ), polygon.begin() + ((indices.at(indices[0] * 9 + 8))));
	//	//std::vector<float> vec2(polygon.begin() + (polygon.at(indices[1] * 9) ), polygon.begin() + ((indices.at(indices[1] * 9 + 8))));
	//	//std::vector<float> vec3(polygon.begin() + (polygon.at(indices[2] * 9) ), polygon.begin() + ((indices.at(indices[2] * 9 + 8))));
	//	//std::vector<float> vec3(polygon.begin() + ((i+2) * 12), polygon.begin() + ((indices.at(2) * 12) + 8));

	//	std::vector<float> combinedVector;

	//	combinedVector.insert(combinedVector.end(), vec1.begin(), vec1.end());
	//	combinedVector.insert(combinedVector.end(), vec2.begin(), vec2.end());
	//	combinedVector.insert(combinedVector.end(), vec3.begin(), vec3.end());

	//	combinedVector = getSurfaceNormal(combinedVector);

	//	normals.insert(normals.end(), combinedVector.begin(), combinedVector.end());
	//	
	//	for (int i = 0; i < 3; ++i) {
	//		unsigned int erasedValue = indices[i];
	//		// You can log the erasedValue using your preferred logging method (e.g., std::cout)
	//		std::cout << "Erased Value: " << erasedValue << std::endl;
	//	}

	//	indices.erase(indices.begin(), indices.begin() + 3);
	//}

	return normals;
}

std::vector<float> getSurfaceNormal(std::vector<float> polygon) {

	std::vector<float> u = { polygon.at(1 * 9) - polygon.at(0), polygon.at(1 * 9 + 1) - polygon.at((1)), polygon.at(1 * 9 + 2) - polygon.at(2) };
	std::vector<float> v = { polygon.at(2 * 9) - polygon.at(0), polygon.at(2 * 9 + 1) - polygon.at((1)), polygon.at(2 * 9 + 2) - polygon.at(2) };

	std::vector<float> normalVector;

	normalVector.push_back((u.at(1) * v.at(2)) - (u.at(2) * v.at(1)));
	normalVector.push_back((u.at(2) * v.at(0)) - (u.at(0) * v.at(2)));
	normalVector.push_back((u.at(0) * v.at(1)) - (u.at(1) * v.at(0)));

	return normalVector;
}