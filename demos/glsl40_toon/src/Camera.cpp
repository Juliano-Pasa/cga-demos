#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>

Camera::Camera(InputManager* inputManager, vec3 position)
{
	this->inputManager = inputManager;

	InitTransform(position);
	this->orientation = vec3(1.0f, 0.0f, 0.0f);
	this->up = vec3(0.0f, 1.0f, 0.0f);

	this->sensitivity = 0.5f;
	this->smoothness = 5.0f;
	this->freeCamMode = false;

	referenceEntity = nullptr;

	GenerateViewMatrix();
}

void Camera::GenerateViewMatrix()
{
	vec3 newOrientation = glm::rotateY(vec3(1.0f, 0.0f, 0.0f), transform.angles().y);
	vec3 uVector = glm::cross(newOrientation, vec3(0.0f, 1.0f, 0.0f));
	
	newOrientation = glm::rotate(newOrientation, transform.angles().x, uVector);
	orientation = glm::normalize(newOrientation);

	vec3 newUp = glm::cross(uVector, orientation);
	up = glm::normalize(newUp);

	viewMatrix = glm::lookAt(transform.position() - orientation * 100.0f, transform.position(), up);
}


#pragma region UpdateFuncions

void Camera::Update(double deltaTime)
{
	transform.position(referenceEntity->transform.position());
	ReadKeyboardInputs((float)deltaTime);
	ReadMouseInputs();
	if (transform.hasChanged())
	{
		GenerateViewMatrix();
	}
}

#pragma endregion


#pragma region InputFunctions

void Camera::ReadKeyboardInputs(float deltaTime)
{
	if (inputManager->GetIsKeyDown(GLFW_KEY_LEFT_CONTROL))
	{
		freeCamMode = true;
	}
	if (!inputManager->GetIsKeyDown(GLFW_KEY_LEFT_CONTROL))
	{
		freeCamMode = false;
		firstMouseMove = true;
	}
}

void Camera::ReadMouseInputs()
{
	if (!freeCamMode)
	{
		return;
	}

	dvec2 mouseCoords = inputManager->GetMouseCoords();

	if (firstMouseMove)
	{
		lastMouseCoords = mouseCoords;
		firstMouseMove = false;
		return;
	}

	dvec2 delta = mouseCoords - lastMouseCoords;
	lastMouseCoords = mouseCoords;

	if (delta.x * delta.x + delta.y * delta.y < 0.001)
	{
		return;
	}

	vec3 currentAngles = transform.angles();
	currentAngles.x -= glm::radians((float) delta.y * sensitivity);
	currentAngles.y -= glm::radians((float) delta.x * sensitivity);

	float maxVerticalAngle = glm::radians(90.0f);
	if (currentAngles.x > maxVerticalAngle)
	{
		currentAngles.x = maxVerticalAngle;
	}
	else if (currentAngles.x < -maxVerticalAngle)
	{
		currentAngles.x = -maxVerticalAngle;
	}

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

#pragma region Getters

const vec3& Camera::CameraAngles()
{
	return transform.angles();
}

const vec3& Camera::CameraPosition()
{
	return transform.position();
}

const vec3& Camera::CameraOrientation()
{
	return orientation;
}

const vec3& Camera::CameraUp()
{
	return up;
}

#pragma endregion

void Camera::SetEntityReference(Entity* reference)
{
	referenceEntity = reference;
	transform.position(referenceEntity->transform.position());
}