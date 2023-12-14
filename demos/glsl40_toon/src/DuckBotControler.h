#pragma once

#include "glslprogram.h"
#include "EntityControler.h"

class DuckBotControler : public EntityControler
{
private:
	vec3 resultingForce;
	float maxForce;
	float movementStrength;

	float maxSpeed;
	float baseSpeed;
	float sprintSpeed;
	vec3 currentSpeed;

	float mass;
	vec3 orientation;

	float wanderAngle;
	float wanderDistance;
	float wanderRadius;
	float wanderJitter;


	void CalculateOrientation();
	vec3 TruncateMagnitude(vec3 vec, float maxMagnitude);
	vec3 Wander();
	void ApplyForces(float deltaTime);

public:
	DuckBotControler();

	void Initialize(float maxForce, float movementStrength, float baseSpeed, float sprintSpeed, float mass);
	void Update(float deltaTime);
};

