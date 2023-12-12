#pragma once

#include "glslprogram.h"
#include "Transform.h"
#include "GLFW\glfw3.h"
#include "Entity.h"

class Camera
{
private:
	dvec2 lastMouseCoords;

	Transform transform;
	vec3 orientation;
	vec3 up;

	Entity* referenceEntity;

	bool firstMouseMove;
	bool freeCamMode;
	void InitTransform(vec3 position);

	void ReadKeyboardInputs(float deltaTime);
	void GenerateViewMatrix();
	void ReadMouseInputs();

	GLFWwindow* window;

public:
	float sensitivity;
	float smoothness;

	mat4 viewMatrix;
	
	Camera(GLFWwindow* window, vec3 position);

	void Update(double deltaTime);
	const vec3& CameraAngles();
	const vec3& CameraPosition();
	const vec3& CameraOrientation();
	const vec3& CameraUp();
	void SetEntityReference(Entity* reference);
};