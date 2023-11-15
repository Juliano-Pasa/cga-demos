#pragma once

#include "glslprogram.h"
#include "Entity.h"
#include "Transform.h"
#include "WorldLight.h"
#include <vector>

using namespace std;

class PlayerCube : public Entity
{
private:
	void GenSphere();
	void GenBuffers();

	GLuint vaoID;
	int quality;
	vector<vec3> vertices;
	vector<vec3> normals;
	vector<vec3> colors;
	vector<unsigned int> indices;

	GLSLProgram shader;

	vec3 lightPos;

public:
	PlayerCube(vec3 position, vec3 rotation, vec3 scale);

	WorldLight* worldLight;

	void Initialize();
	void Update(double deltaTime);
	void Render(mat4 projection, mat4 view);
};