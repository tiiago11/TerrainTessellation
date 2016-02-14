#include "Terrain.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include "TextureManager.h"

using namespace std;

Terrain::Terrain(GLFWwindow* window)
{
	glActiveTexture(GL_TEXTURE0);
	if (!TextureManager::Inst()->LoadTexture("resources/sm3.tif", 10))
		std::cout << "Failed to load texture." << std::endl;
	size = TextureManager::m_TexMap.at(10).size.x / 2.0f;

	camera = new Camera(window);
	lightPos = vec3((float)size, 300.0f, (float)size);
}

void Terrain::genRegularGrid()
{
	std::vector<vec3> vertices;
	//// Vertices
	for (uint32_t i = 0; i < size; i++)
	{
		for (uint32_t j = 0; j < size; j++)
		{
			vertices.emplace_back(i, 0, j);
		}
	}

	// Indices
	for (uint32_t i = 0; i < size - 1; i++)
	{
		for (uint32_t j = 0; j < size - 1; j++)
		{
			indices.emplace_back(size * i + j);
			indices.emplace_back(size * (i + 1) + j);
			indices.emplace_back(size * (i + 1) + j + 1);
			indices.emplace_back(size * i + j + 1);
		}
	}

	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);

	unsigned int handle[2];
	glGenBuffers(2, handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), (GLvoid*)&vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), (GLvoid*)&indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

	glPatchParameteri(GL_PATCH_VERTICES, 3);
}

void Terrain::initMesh()
{
	genRegularGrid();

	// init matrices
	modelMatrix = glm::mat4(1.0f);
	viewMatrix = glm::lookAt(
		vec3(0.0f, 0.0f, -1.0f), //eye
		vec3(0.0f, 0.0f, 0.0f), //center
		vec3(0.0f, 1.0f, 0.0f)); //up
	projectionMatrix = glm::perspective(glm::radians(60.0f), 1.0f, 1.0f, 5000.0f);

	// load shaders
	try {
		shader.compileShader("shader/vtfDisplacement.vert");
		shader.compileShader("shader/vtfDisplacement.frag");
		shader.compileShader("shader/vtfDisplacement.geom", GLSLShader::GLSLShaderType::GEOMETRY);
		shader.compileShader("shader/quadtess.tes", GLSLShader::GLSLShaderType::TESS_EVALUATION);
		shader.compileShader("shader/quadtess.tcs", GLSLShader::GLSLShaderType::TESS_CONTROL);

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
}

void Terrain::update(double deltaTime)
{
	// update the camera
	camera->Update(deltaTime);

	//// matrices setup
	viewMatrix = camera->Look();
	modelMatrix = glm::translate(vec3());
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
	shader.setUniform("Light.Position", viewMatrix * vec4(lightPos, 1.0f)); // Light position in eye coords.
	shader.setUniform("Light.Intensity", 1.0f, 1.0f, 1.0f);

	// material info
	shader.setUniform("Material.Ka", 0.3f, 0.5f, 0.3f); // Ambient reflectivity
	shader.setUniform("Material.Kd", 0.9f, 0.5f, 0.3f); // Diffuse reflectivity
	shader.setUniform("Material.Ks", 0.8f, 0.8f, 0.8f); // Specular reflectivity
	shader.setUniform("Material.Shininess", 100.0f);    // Specular shininess factor
	
	shader.setUniform("uSampler", 0);
	shader.setUniform("uSize", size);

	shader.setUniform("camPos", camera->vEye);
}

void Terrain::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(vaoID);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawElements(GL_PATCHES, indices.size(), GL_UNSIGNED_INT, (GLubyte *)NULL);
	glFinish();
}

void Terrain::resize(int, int)
{
}
