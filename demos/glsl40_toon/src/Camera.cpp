#include "Camera.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>

Camera::Camera(InputManager* inputManager, vec3 position)
{
	this->inputManager = inputManager;

	InitTransform(position);
	this->orientation = vec3(1.0f, 0.0f, 0.0f);
	this->up = vec3(0.0f, 1.0f, 0.0f);

	this->sensitivity = 0.25f;
	this->smoothness = 5.0f;
	this->camDistance = 500.0f;

	this->freeCam = false;
	this->slerpMode = false;
	this->slerpDuration = 0.25;
	this->currentSlerpStep = 0.0;

	this->maxVerticalAngle = glm::radians(-20.0f);
	this->minVerticalAngle = glm::radians(-90.0f);

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

	viewMatrix = glm::lookAt(transform.position() - orientation * camDistance, transform.position(), up);
}


#pragma region UpdateFuncions

void Camera::Update(double deltaTime)
{
	transform.position(referenceEntity->transform.position());

	if (slerpMode)
	{
		SlerpQuaternion(deltaTime);
	}
	else
	{
		ReadKeyboardInputs();
		ReadMouseInputs();
	}

	if (transform.hasChanged())
	{
		GenerateViewMatrix();
	}
}

#pragma endregion


#pragma region InputFunctions

void Camera::ReadKeyboardInputs()
{
	if (inputManager->GetIsKeyDown(GLFW_KEY_LEFT_CONTROL))
	{
		freeCam = true;
	}
	else if (freeCam && !slerpMode)
	{
		SetupSlerp();
	}
}

void Camera::ReadMouseInputs()
{
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

	if (currentAngles.x > maxVerticalAngle)
	{
		currentAngles.x = maxVerticalAngle;
	}
	else if (currentAngles.x < minVerticalAngle)
	{
		currentAngles.x = minVerticalAngle;
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

void Camera::SlerpQuaternion(double deltaTime)
{
	float interpolationValue = glm::sin(glm::pi<float>() * currentSlerpStep / 2.0);
	float newYAngle = initialSlerpY * (1 - interpolationValue) + finalSlerpY * interpolationValue;

	vec3 currentAngles = transform.angles();
	currentAngles.y = newYAngle;
	transform.angles(currentAngles);

	currentSlerpStep += deltaTime / slerpDuration;

	if (currentSlerpStep >= 1.0f)
	{
		currentSlerpStep = 0.0;
		slerpMode = false;
	}
}

void Camera::SetupSlerp()
{
	initialSlerpY = transform.angles().y;
	finalSlerpY = referenceEntity->transform.angles().y;

	if (abs(finalSlerpY - initialSlerpY) > glm::pi<float>())
	{
		if (initialSlerpY < 0)
		{
			initialSlerpY += glm::two_pi<float>();
		}
		else if (finalSlerpY < 0)
		{
			finalSlerpY += glm::two_pi<float>();
		}
	}

	slerpMode = true;
	freeCam = false;
}