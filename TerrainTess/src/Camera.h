#pragma once
#include "GL/glew.h"
#include "glslprogram.h"
#include "GLFW\glfw3.h"

class Camera
{
public:
	glm::vec3 vEye, vView, vUp;
	float runModifierAmount;
	float mouseSpeed;
	float mouseSensitivity; // How many degrees to rotate per pixel moved by mouse (nice value is 0.10)

	// Main functions
	void RotateWithMouse();
	void Update(double deltaTime);
	glm::mat4 Look();

	void SetMovingKeys(int a_iForw, int a_iBack, int a_iLeft, int a_iRight);	

	// Functions that get viewing angles
	float GetAngleX();
	float GetAngleY();

	// Constructors
	Camera(GLFWwindow* window);

private:
	GLFWwindow* window;
	char iForw, iBack, iLeft, iRight;
	double mouseX, mouseY;
};