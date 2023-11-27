#pragma once

#include "glslprogram.h"
#include "Transform.h"
#include "GLFW\glfw3.h"

class Camera
{
private:
	dvec2 lastMouseCoords;

	Transform transform;
	vec3 orientation;
	vec3 up;

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
	float speed;
	float baseSpeed;
	float acceleration;

	mat4 viewMatrix;
	
	Camera(GLFWwindow* window, vec3 position);

	void Update(double deltaTime);
	const vec3& CameraPosition();
};