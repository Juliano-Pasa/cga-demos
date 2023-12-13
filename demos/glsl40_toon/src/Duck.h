#pragma once

#include "glslprogram.h"
#include "Entity.h"
#include "WorldLight.h"
#include "Camera.h"
#include "InputManager.h"
#include <vector>

using namespace std;

class Duck : public Entity
{
private:
	void GenerateModel();
	void GenerateBuffers();

	GLuint primitiveRestartIndex;
	GLuint vaoID;

	vector<vec3> vertices;
	vector<vec3> colors;
	vector<unsigned int> indices;

	Camera* camera;
	GLSLProgram shader;
	WorldLight* worldLight;

	InputManager* inputManager;
	void ReadKeyboardInputs(float deltaTime);
	void ReadMouseInputs();


	bool holdRotation;
	bool firstMouseMove;
	dvec2 lastMouseCoords;
	bool playerControled;

	vec3 steeringForce;
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

	void ApplyForces(float deltaTime);
	void CalculateOrientation();
	vec3 TruncateMagnitude(vec3 vec, float maxMagnitude);

public:
	Duck(vec3 position, vec3 scale, WorldLight* worldLight, Camera* camera, InputManager* inputManager, bool playerControled);

	void Initialize();
	void Update(double deltaTime);
	void Render(mat4 projection, mat4 view);
};
