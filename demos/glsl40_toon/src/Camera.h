#pragma once

#include "glslprogram.h"
#include "Transform.h"
#include "InputManager.h"
#include "Entity.h"

class Camera
{
private:
	Transform transform;
	vec3 orientation;
	vec3 up;

	Entity* referenceEntity;

	dvec2 lastMouseCoords;
	bool firstMouseMove;
	bool freeCamMode;

	float maxVerticalAngle;
	float minVerticalAngle;

	void InitTransform(vec3 position);

	void ReadKeyboardInputs(float deltaTime);
	void GenerateViewMatrix();
	void ReadMouseInputs();

	InputManager* inputManager;

public:
	float sensitivity;
	float smoothness;
	float camDistance;

	mat4 viewMatrix;
	
	Camera(InputManager* inputManager, vec3 position);

	void Update(double deltaTime);
	const vec3& CameraAngles();
	const vec3& CameraPosition();
	const vec3& CameraOrientation();
	const vec3& CameraUp();
	void SetEntityReference(Entity* reference);
};