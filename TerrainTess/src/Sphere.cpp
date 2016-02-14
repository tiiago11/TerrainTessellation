#include "Sphere.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

using namespace std;

Sphere::Sphere(GLFWwindow* window, int quality){
	this->quality = quality;
	this->window = window;
	camera = new Camera(window);
	lightPos = vec3(0.0f, 2.0f, -2.0f);
	spherePos = vec3(0.0f, 0.0f, 2.5f);
}

void Sphere::initMesh(){
	//genSphere();
	genTriangle();
	genBuffers();

	// init matrices
	modelMatrix = glm::mat4(1.0f);
	viewMatrix = glm::lookAt(
		vec3(0.0f, 0.0f, -1.0f), //eye
		vec3(0.0f, 0.0f, 0.0f), //center
		vec3(0.0f, 1.0f, 0.0f)); //up
	projectionMatrix = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 100.0f);

	// load shaders
	try {
		shader.compileShader("shader/vtfDisplacement.vert");
		shader.compileShader("shader/vtfDisplacement.frag");
		
		shader.link();
		shader.use();
	}
	catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		system("pause");
		exit(EXIT_FAILURE);
	}
	shader.printActiveAttribs();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void Sphere::update(double deltaTime){
	// move the light pos
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
		lightPos.z += 0.01f;
	}else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
		lightPos.z -= 0.01f;
	} // translate the model
	else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
		spherePos.z += 0.01f;
	}
	else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
		spherePos.z -= 0.01f;
	}

	// update the camera
	camera->Update(deltaTime);

	//// matrices setup
	viewMatrix = camera->Look();
	modelMatrix = glm::translate(spherePos);
	modelViewMatrix = viewMatrix * modelMatrix;
	modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

	// matrices
	shader.setUniform("MVP", modelViewProjectionMatrix); //ModelViewProjection
	shader.setUniform("ModelViewMatrix", modelViewMatrix); // ModelView Matrix

	///////////////////////////////////// NORMAL MATRIX 
	//   The normal matrix is typically the inverse transpose of the upper-left 3x3
	// portion of the modelView matrix. We use the inverse transpose because
	// normal vectors transform differently than the vertex position vectors.
	glm::mat3 nm = glm::mat3(glm::inverse(glm::transpose(modelViewMatrix)));
	shader.setUniform("NormalMatrix", nm); // Normal Matrix
	// OR
	//   If your modelView matrix does not include any non-uniform scalings, 
	// then one can use the upper-left 3x3 of the modelView matrix in place
	// of the normal matrix to transform your normal. 
	// IN THIS EXAMPLE, THEY ARE EQUIVALENT!!!
	//shader.setUniform("NormalMatrix", mat3(mv)); // Normal Matrix
	/////////////////////// END NORMAL MATRIX
	
	// light info
	shader.setUniform("Light.Position",  viewMatrix * vec4(lightPos, 1.0f)); // Light position in eye coords.
	shader.setUniform("Light.Intensity", 1.5f, 1.5f, 1.5f);

	// material info
	shader.setUniform("Material.Ka", 0.3f, 0.5f, 0.3f); // Ambient reflectivity
	shader.setUniform("Material.Kd", 0.9f, 0.5f, 0.3f); // Diffuse reflectivity
	shader.setUniform("Material.Ks", 0.8f, 0.8f, 0.8f); // Specular reflectivity
	shader.setUniform("Material.Shininess", 100.0f);    // Specular shininess factor
}

void Sphere::render(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(vaoID);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (GLubyte *)NULL);
	glBindVertexArray(0);
}

void Sphere::genBuffers(){

	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);

	unsigned int handle[4];
	glGenBuffers(4, handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), (GLvoid*)&vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
	glEnableVertexAttribArray(0);  // Vertex position -> layout 0 in the VS

	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), (GLvoid*)&colors[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
	glEnableVertexAttribArray(1);  // Vertex color -> layout 1 in the VS

	glBindBuffer(GL_ARRAY_BUFFER, handle[2]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), (GLvoid*)&normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)2, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
	glEnableVertexAttribArray(2);  // Vertex normal -> layout 2 in the VS

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), (GLvoid*)&indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void Sphere::resize(int x, int y){

}

void Sphere::genSphere(){
	vec3 v;
	vec3 n;

	double tetaStep = (2 * glm::pi<float>()) / (double)quality;
	double fiiStep = glm::pi<float>() / (double)quality;

	int tetacount = 0;
	int fiicount = 0;
	int i = 0;
	/// Generates vetrice data
	for (double teta = 0.0; teta <= (2 * glm::pi<float>()); teta += tetaStep)
	{
		tetacount++;
		fiicount = 0;
		for (double fii = 0.0; fii < glm::pi<float>(); fii += fiiStep)
		{
			v[0] = cos(teta) * sin(fii);
			v[1] = sin(teta) * sin(fii);
			v[2] = cos(fii);
			n[0] = v[0];
			n[1] = v[1];
			n[2] = v[2];

			vertices.push_back(v);
			normals.push_back(n);

			fiicount++;
			if (fiicount >= quality)
				break;
		}
		v[0] = cos(teta) * sin(glm::pi<float>());
		v[1] = sin(teta) * sin(glm::pi<float>());
		v[2] = cos(glm::pi<float>());
		n[0] = v[0];
		n[1] = v[1];
		n[2] = v[2];

		vertices.push_back(v);
		normals.push_back(n);

		if (tetacount > quality)
			break;
	}

	/// Make indexes
	int qualMaisUm = quality + 1;
	for (int i = 0; i < quality - 1; i++)
	{
		for (int j = 0; j < qualMaisUm - 1; j++)
		{
			indices.push_back(i       * qualMaisUm + j);
			indices.push_back((i + 1) * qualMaisUm + j + 1);
			indices.push_back((i + 1) * qualMaisUm + j);

			indices.push_back(i       * qualMaisUm + j);
			indices.push_back(i       * qualMaisUm + j + 1);
			indices.push_back((i + 1) * qualMaisUm + j + 1);
		}
	}
	for (int i = 0; i < quality; i++)
	{
		indices.push_back((quality - 1)  * qualMaisUm + i);
		indices.push_back(0 + i + 1);
		indices.push_back(0 + i);

		indices.push_back((quality - 1) * qualMaisUm + i);
		indices.push_back((quality - 1) * qualMaisUm + i + 1);
		indices.push_back(0 + i + 1);
	}

	//init colors
	for (int i = 0; i < vertices.size(); i++){
		colors.push_back(vec3(1.0f, 0.0f, 0.0f));
	}
}

void Sphere::genTriangle()
{
	vertices.emplace_back(0.0f, 1.0f, 0.0);
	vertices.emplace_back(1.0f, -1.0f, 0.0);
	vertices.emplace_back(-1.0f, -1.0f, 0.0);

	normals.emplace_back(0.0f, 0.0f, 1.0);
	normals.emplace_back(0.0f, 0.0f, 1.0);
	normals.emplace_back(0.0f, 0.0f, 1.0);


	indices.emplace_back(0);
	indices.emplace_back(1);
	indices.emplace_back(2);

	//init colors
	for (int i = 0; i < vertices.size(); i++) {
		colors.push_back(vec3(1.0f, 0.0f, 0.0f));
	}
}