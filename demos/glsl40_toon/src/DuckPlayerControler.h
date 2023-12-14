#pragma once

#include "glslprogram.h"
#include "EntityControler.h"
#include "InputManager.h"
#include "Camera.h"

class DuckPlayerControler : public EntityControler
{
	InputManager* inputManager;
	void ReadKeyboardInputs();
	void ReadMouseInputs();

	bool holdRotation;
	bool firstMouseMove;
	dvec2 lastMouseCoords;

	vec3 resultingForce;
	float maxForce;
	float movementStrength;

	float maxSpeed;
	float baseSpeed;
	float sprintSpeed;
	vec3 currentSpeed;

	float mass;

	vec3 orientation;
	vec3 sideOrientation;

	Camera* camera;

	void CalculateOrientation();
	void ApplyForces(float deltaTime);
	vec3 TruncateMagnitude(vec3 vec, float maxMagnitude);

public:
	DuckPlayerControler();

	void Initialize(InputManager* inputManager, Camera* camera, float maxForce, float movementStrength, float baseSpeed, float sprintSpeed, float mass);
	void Update(float deltaTime);
};