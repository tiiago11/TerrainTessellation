/*
	GLSL 4.x demo
	Mostra o uso de:
	-GLM - Math library
	-Phong shading
	-Procedural sphere generation using spheric coordinates
	-VBO & VAO
	-GL error detection
	-glfwGetKeyOnce


	August 2015 - Tiago Augusto Engel - tengel@inf.ufsm.br
*/


//Include GLEW - always first 
#include "GL/glew.h"
#include <GLFW/glfw3.h>

//Include the standard C++ headers 
#include "glutils.h"
#include "Mesh.h"
#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>
#include "Sphere.h"
#include "TextureManager.h"
#include "RegularGrid.h"

Mesh *scene;
GLFWwindow* window;
bool wireframe = false;
TextureManager* texManager;


//add to glfwGetKey that gets the pressed key only once (not several times)
char keyOnce[GLFW_KEY_LAST + 1];
#define glfwGetKeyOnce(WINDOW, KEY)             \
    (glfwGetKey(WINDOW, KEY) ?              \
     (keyOnce[KEY] ? false : (keyOnce[KEY] = true)) :   \
     (keyOnce[KEY] = false))

// Define an error callback  
void errorCallback(int error, const char* description)
{
	fputs(description, stderr);
	_fgetchar();
}

// Key cb
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void mainLoop() 
{
	double thisTime;
	double lastTime = glfwGetTime();
	do
	{
		GLUtils::checkForOpenGLError(__FILE__, __LINE__);

		// Toggle wireframe
		if (glfwGetKeyOnce(window, GLFW_KEY_F)) 
		{
			wireframe = !wireframe;
			if (wireframe) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
			else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
		}

		// set deltatime and call update
		thisTime = glfwGetTime();
		scene->update(thisTime - lastTime);
		lastTime = thisTime;

		scene->render();

		glfwSwapBuffers(window);
		//Get and organize events, like keyboard and mouse input, window resizing, etc...  
		glfwPollEvents();
	} 
	while (!glfwWindowShouldClose(window));
}

void initGLFW()
{
	// Initialize GLFW  
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	window = glfwCreateWindow(1000, 1000, "WTF", NULL, NULL);
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		system("pause");
		exit(EXIT_FAILURE);
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(window, 0, 0);

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetErrorCallback(errorCallback);
}

void initGL()
{
	// Initialize GLEW
	glewExperimental = GL_TRUE; //ensures that all extensions with valid entry points will be exposed.
	GLenum err = glewInit();
	GLUtils::checkForOpenGLError(__FILE__, __LINE__); // will throw error. Just ignore, glew bug
	if (err != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		system("pause");
		exit(EXIT_FAILURE);
	}
	GLUtils::dumpGLInfo();

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	glDebugMessageCallback(GLUtils::debugCallback, NULL);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
	glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0, GL_DEBUG_SEVERITY_NOTIFICATION, -1, "Start debugging");
}

void close()
{
	//Close OpenGL window and terminate GLFW  
	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}

int main(void)
{
	initGLFW();

	initGL();

	texManager = TextureManager::Inst();

	glActiveTexture(GL_TEXTURE0);
	if (!texManager->LoadTexture("resources/sm3.tif", 10))
		std::cout << "Failed to load texture." << std::endl;

	scene = new RegularGrid(window, TextureManager::m_TexMap.at(10).size.x);
	scene->initMesh();

	mainLoop();
	
	close();

	return 0;
}