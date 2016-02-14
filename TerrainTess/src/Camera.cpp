#include "Camera.h"

#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
using namespace std;

const float PI = float(atan(1.0)*4.0);

Camera::Camera(GLFWwindow* window)
	: runModifierAmount(2.0f)
{
	this->window = window;
	vEye = glm::vec3(0.0f, 0.0f, 0.0f);
	vView = glm::vec3(0.0f, 0.0, 10.0f);
	vUp = glm::vec3(0.0f, 1.0f, 0.0f);
	mouseSpeed = 1.0f;
	mouseSensitivity = 0.001f;
	this->SetMovingKeys('W', 'S', 'A', 'D');
}

/*
	Checks for moving of mouse and rotates camera.
	*/
void Camera::RotateWithMouse()
{
	int left, top, right, bottom;
	glfwGetWindowFrameSize(window, &left, &top, &right, &bottom);
	glfwGetCursorPos(window, &mouseX, &mouseY);

	float iCentX = (left + right) >> 1;
	float iCentY = (top + bottom) >> 1;
	float deltaX = (iCentX - float(mouseX)) * mouseSensitivity;
	float deltaY = (iCentY - float(mouseY)) * mouseSensitivity;

	if (deltaX != 0.0f)
	{
		vView -= vEye;
		vView = glm::rotate(vView, deltaX, glm::vec3(0.0f, 1.0f, 0.0f));
		vView += vEye;
	}
	if (deltaY != 0.0f)
	{
		glm::vec3 vAxis = glm::cross(vView - vEye, vUp);
		vAxis = glm::normalize(vAxis);
		float fAngle = deltaY;
		float fNewAngle = fAngle + GetAngleX();
		if (fNewAngle > -89.80f && fNewAngle < 89.80f)
		{
			vView -= vEye;
			vView = glm::rotate(vView, deltaY, vAxis);
			vView += vEye;
		}
	}
	glfwSetCursorPos(window, iCentX, iCentY);
}

/*
	Gets Y angle of camera (head turning left and right).
	*/
float Camera::GetAngleY()
{
	glm::vec3 vDir = vView - vEye;
	vDir.y = 0.0f;
	glm::normalize(vDir);
	float fAngle = acos(glm::dot(glm::vec3(0, 0, -1), vDir))*(180.0f / PI);
	if (vDir.x < 0)fAngle = 360.0f - fAngle;
	return fAngle;
}

/*
	Gets X angle of camera (head turning up and down).
*/
float Camera::GetAngleX()
{
	glm::vec3 vDir = vView - vEye;
	vDir = glm::normalize(vDir);
	glm::vec3 vDir2 = vDir; vDir2.y = 0.0f;
	vDir2 = glm::normalize(vDir2);
	float fAngle = acos(glm::dot(vDir2, vDir))*(180.0f / PI);
	if (vDir.y < 0)fAngle *= -1.0f;
	return fAngle;
}
/*
	Set keyboard moving keys
*/
void Camera::SetMovingKeys(int forward, int back, int left, int right)
{
	iForw = forward;
	iBack = back;
	iLeft = left;
	iRight = right;
}

/*
	Performs updates of camera - moving and rotating.
	*/

void Camera::Update(double deltaTime)
{
	RotateWithMouse();


	if (glfwGetKey(window, GLFW_KEY_Q))
		mouseSpeed += mouseSpeed * 2.f * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_E))
		mouseSpeed -= mouseSpeed * 2.f * deltaTime;


	float amount = mouseSpeed;

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
		amount *= runModifierAmount;

	// Get view direction
	glm::vec3 vMove = vView - vEye;
	vMove = glm::normalize(vMove);
	vMove *= amount;

	glm::vec3 vStrafe = glm::cross(vView - vEye, vUp);
	vStrafe = glm::normalize(vStrafe);
	vStrafe *= amount;

	int iMove = 0;
	glm::vec3 vMoveBy;
	//Get vector of move
	if (glfwGetKey(window, iForw))
		vMoveBy += vMove * float(deltaTime) * 50.0f;
	if (glfwGetKey(window, iBack))
		vMoveBy -= vMove * float(deltaTime);
	if (glfwGetKey(window, iLeft))
		vMoveBy -= vStrafe * float(deltaTime);
	if (glfwGetKey(window, iRight))
		vMoveBy += vStrafe * float(deltaTime);
	vEye += vMoveBy;
	vView += vMoveBy;
}

/*
	Returns proper view matrix to make camera look.
	*/
glm::mat4 Camera::Look()
{
	return glm::lookAt(vEye, vView, vUp);
}