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

	float speed;
	float baseSpeed;
	float acceleration;


	vec3 resultingForce;

public:
	Duck(vec3 position, vec3 scale, WorldLight* worldLight, Camera* camera, InputManager* inputManager);

	void Initialize();
	void Update(double deltaTime);
	void Render(mat4 projection, mat4 view);
};
