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

	this->wobbleAngle = 0.0f;
	this->wobble = true;
	this->maxWobble = glm::radians(10.0f);
	this->wobbleSpeed = glm::pi<float>() * 6;
}

void DuckBotControler::Initialize(float maxForce, float movementStrength, float baseSpeed, float sprintSpeed, float mass, vec3 goalPosition, Wind* wind)
{
	this->maxForce = maxForce;
	this->movementStrength = movementStrength;
	this->baseSpeed = baseSpeed;
	this->sprintSpeed = sprintSpeed;
	this->mass = mass;

	this->maxSpeed = baseSpeed;
	this->goalPosition = goalPosition;

	this->wind = wind;
	this->maxDistanceFromWind = 1000.0f;

	this->currentSeekTime = 0.0f;
	this->seekDuration = 6.0f;

	this->wanderRadius = 50.0f;
	this->wanderDistance = 1.0f;
	this->wanderAngle = glm::half_pi<float>();

	this->currentState = WANDERING;

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

void DuckBotControler::ApplyWobble(float deltaTime)
{
	float rotation = sin(wobbleAngle);

	vec3 currentAngles = entity->transform.angles();
	currentAngles.x = rotation * maxWobble;
	entity->transform.angles(currentAngles);

	wobbleAngle += deltaTime * wobbleSpeed;
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

vec3 DuckBotControler::Seek()
{
	vec3 desiredVelocity = goalPosition - entity->transform.position();
	desiredVelocity.y = 0;
	desiredVelocity = TruncateMagnitude(desiredVelocity, maxSpeed);

	return desiredVelocity - currentSpeed;
}

#pragma endregion

void DuckBotControler::Update(float deltaTime)
{
	if (entity == nullptr || !initialized)
	{
		return;
	}

	currentState = GetCurrentState(deltaTime);

	CalculateOrientation();
	resultingForce += CalculateCurrentForce();

	ApplyForces(deltaTime);
	ApplyWobble(deltaTime);
}

DuckBotState DuckBotControler::GetCurrentState(float deltaTime)
{
	if (currentState == SEEKING)
	{
		if (currentSeekTime < seekDuration)
		{
			maxSpeed = sprintSpeed;
			currentSeekTime += deltaTime;
			return SEEKING;
		}

		currentSeekTime = 0.0f;
		maxSpeed = baseSpeed;
		return WANDERING;
	}

	if (!wind->render)
	{
		maxSpeed = baseSpeed;
		return WANDERING;
	}

	float distanceFromWind = glm::length(wind->transform.position() - entity->transform.position());
	if (distanceFromWind > maxDistanceFromWind)
	{
		maxSpeed = baseSpeed;
		return WANDERING;
	}

	maxSpeed = sprintSpeed;
	return SEEKING;
}

vec3 DuckBotControler::CalculateCurrentForce()
{
	switch (currentState)
	{
	case WANDERING: 
		return Wander();
		break;
	case SEEKING:
		return Seek();
		break;
	default:
		return Wander();			
	}
}
