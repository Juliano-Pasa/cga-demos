#include "DuckBotControler.h"
#include "glm/gtx/vector_angle.hpp"
#include "glm/gtc/constants.hpp"
#include <time.h>
#include <random>
#include <iostream>

DuckBotControler::DuckBotControler() : EntityControler()
{
	resultingForce = vec3(0);
	currentSpeed = vec3(0);
	orientation = vec3(1, 0, 0);
}

void DuckBotControler::Initialize(float maxForce, float movementStrength, float baseSpeed, float sprintSpeed, float mass)
{
	this->maxForce = maxForce;
	this->movementStrength = movementStrength;
	this->baseSpeed = baseSpeed;
	this->sprintSpeed = sprintSpeed;
	this->mass = mass;

	this->maxSpeed = baseSpeed;

	this->wanderRadius = 50.0f;
	this->wanderDistance = 1.0f;
	this->wanderJitter = 0.5f;
	this->wanderAngle = glm::half_pi<float>();


	srand(time(NULL));
	this->initialized = true;
}

vec3 DuckBotControler::TruncateMagnitude(vec3 vec, float maxMagnitude)
{
	if (glm::length(vec) <= maxMagnitude)
	{
		return vec;
	}
	return glm::normalize(vec) * maxMagnitude;
}

void DuckBotControler::CalculateOrientation()
{
	if (glm::length(currentSpeed) < 0.01)
	{
		return;
	}

	orientation = glm::normalize(currentSpeed);
	float angle = atan2(orientation.x, orientation.z);

	vec3 angles = entity->transform.angles();
	angles.y = angle - glm::half_pi<float>();
	entity->transform.angles(angles);
}

void DuckBotControler::ApplyForces(float deltaTime)
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

#pragma region SteeringBehaviour

vec3 DuckBotControler::Wander()
{
	float angle = wanderAngle + entity->transform.angles().y;
	vec3 target = orientation * wanderDistance;

	target += vec3(cos(angle) * wanderRadius, 0, sin(angle) * wanderRadius);

	float random = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
	random /= 5.0f;
	wanderAngle += random;

	return target;
}

#pragma endregion

void DuckBotControler::Update(float deltaTime)
{
	if (entity == nullptr || !initialized)
	{
		return;
	}

	CalculateOrientation();
	resultingForce += Wander();

	ApplyForces(deltaTime);
}
