#pragma once

#include "GL/glew.h"
#include "Mesh.h"
#include "GLFW\glfw3.h"
#include "GLSLProgram.h"
#include <vector>
#include "glm\glm.hpp"
#include "Camera.h"

class Terrain : public Mesh
{
public:
	GLFWwindow* window;
	
	GLSLProgram shader;
	
	int size;
	std::vector<vec3> vertices;
	std::vector<uint32_t> indices;
	GLuint vaoID;
	GLuint sampler = 0;

	Camera* camera;

	glm::mat4 modelMatrix;
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	glm::mat4 modelViewProjectionMatrix;
	glm::mat4 modelViewMatrix;

	vec3 lightPos;

	Terrain(GLFWwindow* window);
	void genRegularGrid();


	void initMesh();
	void update(double t);
	void render();
	void resize(int, int);

};