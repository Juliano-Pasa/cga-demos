#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

Camera::Camera(GLFWwindow* window, vec3 position)
{
	this->window = window;

	InitTransform(position);
	this->orientation = vec3(1.0f, 0.0f, 0.0f);
	this->up = vec3(0.0f, 1.0f, 0.0f);

	this->sensitivity = 100.0f;
	this->smoothness = 5.0f;
	this->speed = 5.0f;
	this->freeCamMode = false;

	this->viewMatrix = glm::lookAt(position, position + orientation, up);
}

void Camera::GenerateViewMatrix()
{
	vec3 newOrientation = glm::rotateY(vec3(1.0f, 0.0f, 0.0f), transform.angles().y);
	vec3 uVector = glm::cross(newOrientation, vec3(0.0f, 1.0f, 0.0f));
	
	newOrientation = glm::rotate(newOrientation, transform.angles().x, uVector);
	orientation = glm::normalize(newOrientation);

	vec3 newUp = glm::cross(orientation, uVector);
	up = glm::normalize(newUp);

	viewMatrix = glm::lookAt(transform.position(), transform.position() + orientation, up);
}


#pragma region UpdateFuncions

void Camera::Update(double deltaTime)
{
	ReadKeyboardInputs((float)deltaTime);
	ReadMouseInputs();
	GenerateViewMatrix();
}

#pragma endregion


#pragma region InputFunctions

void Camera::ReadKeyboardInputs(float deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		transform.position(transform.position() + speed * deltaTime * orientation);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		transform.position(transform.position() - speed * deltaTime * orientation);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		transform.position(transform.position() + speed * deltaTime * glm::normalize(glm::cross(orientation, up)));
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		transform.position(transform.position() - speed * deltaTime * glm::normalize(glm::cross(orientation, up)));
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		freeCamMode = true;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)
	{
		freeCamMode = false;
	}
}

void Camera::ReadMouseInputs()
{
	if (!freeCamMode)
	{
		return;
	}

	double xCoord, yCoord;
	glfwGetCursorPos(window, &xCoord, &yCoord);

	if (firstMouseMove)
	{
		lastMouseCoords = dvec2(xCoord, yCoord);
		firstMouseMove = false;
		return;
	}

	dvec2 delta = dvec2(xCoord, yCoord) - lastMouseCoords;
	lastMouseCoords.x = xCoord;
	lastMouseCoords.y = yCoord;

	vec3 currentAngles = transform.angles();
	currentAngles.x += (float) yCoord * sensitivity;
	currentAngles.y += (float) xCoord * sensitivity;
	transform.angles(currentAngles);
}

#pragma endregion


#pragma region InitFunctions

void Camera::InitTransform(vec3 position)
{
	transform.position(position);
	transform.scale(vec3(1.0f));
	transform.angles(vec3(0.0f));
	transform.rotation(quat(transform.angles()));
}

#pragma endregion
