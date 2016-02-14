#ifndef VBOSPHERE_H
#define VBOSPHERE_H

#include "GL/glew.h"

#include "Mesh.h"
#include <vector>
#include "glslprogram.h"
#include "Camera.h"

class Sphere : public Mesh 
{
public:
	Sphere(GLFWwindow* window, int quality = 50);
	
	// mesh virtual functions
	void initMesh();
	void update(double t);
	void render();
	void resize(int, int);

private:
	void genSphere();
	void genTriangle();
	void genBuffers();

	GLuint vaoID;
	int quality;
	std::vector<vec3> vertices;
	std::vector<vec3> normals;
	std::vector<vec3> colors;
	std::vector<unsigned int> indices;
	
	GLSLProgram shader;
	Camera* camera;
	GLFWwindow* window;

	glm::mat4 modelMatrix;
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	glm::mat4 modelViewProjectionMatrix;
	glm::mat4 modelViewMatrix;

	vec3 lightPos;
	vec3 spherePos;
};

#endif