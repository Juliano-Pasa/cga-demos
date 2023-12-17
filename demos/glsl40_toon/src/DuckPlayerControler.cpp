#include "DuckPlayerControler.h"
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

DuckPlayerControler::DuckPlayerControler() : EntityControler()
{
	holdRotation = false;
	firstMouseMove = true;

	resultingForce = vec3(0);
	currentSpeed = vec3(0);

	this->wobbleAngle = 0.0f;
	this->wobble = false;
	this->maxWobble = glm::radians(10.0f);
	this->wobbleSpeed = glm::pi<float>() * 6;
}

void DuckPlayerControler::Initialize(InputManager* inputManager, Camera* camera, float maxForce, float movementStrength, float baseSpeed, float sprintSpeed, float mass)
{
	this->inputManager = inputManager;
	this->camera = camera;

	this->maxForce = maxForce;
	this->movementStrength = movementStrength;
	this->baseSpeed = baseSpeed;
	this->sprintSpeed = sprintSpeed;
	this->mass = mass;

	this->maxSpeed = baseSpeed;
	initialized = true;
}

void DuckPlayerControler::Update(float deltaTime)
{
	if (entity == nullptr || !initialized)
	{
		return;
	}

	ReadMouseInputs();
	ReadKeyboardInputs();
	ApplyForces(deltaTime);

	if (wobble)
	{
		ApplyWobble(deltaTime);
		wobble = false;
	}
	else
	{
		wobbleAngle = 0.0;
		ApplyWobble(deltaTime);
		wobbleAngle = 0.0;
	}
}

void DuckPlayerControler::ApplyForces(float deltaTime)
{
	resultingForce = TruncateMagnitude(resultingForce, maxForce);

	vec3 acceleration = resultingForce / mass;
	currentSpeed += (acceleration * deltaTime);

	currentSpeed = TruncateMagnitude(currentSpeed, maxSpeed);
	entity->transform.position(entity->transform.position() + currentSpeed * deltaTime);

	if (glm::length(resultingForce) > 1.0f)
	{
		resultingForce -= resultingForce / 4.0f;
		return;
	}

	resultingForce = vec3(0);
	if (glm::length(currentSpeed) > 1.0f)
	{
		currentSpeed -= currentSpeed / 10.0f;
		return;
	}
	currentSpeed = vec3(0);
}

void DuckPlayerControler::ApplyWobble(float deltaTime)
{
	float rotation = sin(wobbleAngle);

	vec3 currentAngles = entity->transform.angles();
	currentAngles.x = rotation * maxWobble;
	entity->transform.angles(currentAngles);

	wobbleAngle += deltaTime * wobbleSpeed;
}

void DuckPlayerControler::CalculateOrientation()
{
	vec3 newOrientation = glm::rotateY(vec3(1.0f, 0.0f, 0.0f), entity->transform.angles().y);
	orientation = glm::normalize(newOrientation);

	sideOrientation = glm::rotateY(orientation, glm::radians(-90.0f));
}

vec3 DuckPlayerControler::TruncateMagnitude(vec3 vec, float maxMagnitude)
{
	if (glm::length(vec) <= maxMagnitude)
	{
		return vec;
	}
	return glm::normalize(vec) * maxMagnitude;
}

#pragma region InputFunctions

void DuckPlayerControler:: ReadKeyboardInputs()
{
	if (inputManager->GetIsKeyDown(GLFW_KEY_W))
	{
		resultingForce += movementStrength * orientation;
		wobble = true;
	}
	if (inputManager->GetIsKeyDown(GLFW_KEY_S))
	{
		resultingForce -= movementStrength * orientation;
		wobble = true;
	}
	if (inputManager->GetIsKeyDown(GLFW_KEY_A))
	{
		resultingForce -= movementStrength * sideOrientation;
		wobble = true;
	}
	if (inputManager->GetIsKeyDown(GLFW_KEY_D))
	{
		resultingForce += movementStrength * sideOrientation;
		wobble = true;
	}

	if (inputManager->GetIsKeyDown(GLFW_KEY_LEFT_SHIFT))
	{
		maxSpeed = sprintSpeed;
	}
	if (!inputManager->GetIsKeyDown(GLFW_KEY_LEFT_SHIFT))
	{
		maxSpeed = baseSpeed;
	}

	if (inputManager->GetIsKeyDown(GLFW_KEY_LEFT_CONTROL))
	{
		holdRotation = true;
		firstMouseMove = true;
	}
	if (!inputManager->GetIsKeyDown(GLFW_KEY_LEFT_CONTROL))
	{
		holdRotation = false;
	}
}

void DuckPlayerControler::ReadMouseInputs()
{
	if (holdRotation)
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

	if (delta.x * delta.x < 0.001)
	{
		return;
	}

	vec3 currentAngles = entity->transform.angles();
	currentAngles.y -= glm::radians((float)delta.x * camera->sensitivity);

	entity->transform.angles(currentAngles);

	CalculateOrientation();
}

#pragma endregion
