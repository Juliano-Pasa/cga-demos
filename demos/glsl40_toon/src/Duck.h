#pragma once

#include "glslprogram.h"
#include "Entity.h"
#include "WorldLight.h"
#include "InputManager.h"
#include "EntityControler.h"
#include <vector>

using namespace std;

class Duck : public Entity
{
private:
	void GenerateModel();
	void GenerateBuffers();

	GLuint primitiveRestartIndex;
	GLuint vaoID;

	vector<vec3> colors;
	vector<vec3> normals;
	vector<vec3> vertices;
	vector<unsigned int> indices;

	GLSLProgram shader;
	WorldLight* worldLight;
	EntityControler* entityControler;

	void LoadModel(string path);

public:
	Duck(vec3 position, vec3 scale, WorldLight* worldLight, EntityControler* entityControler);

	void Initialize();
	void Update(double deltaTime);
	void Render(mat4 projection, mat4 view);
};
