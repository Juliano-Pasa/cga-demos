#pragma once

#include "Entity.h"
#include "TextureManager.h"
#include <string>
#include <vector>

using namespace std;

class Terrain : public Entity
{
private:
	int width;
	int height;
	int scaleDown;

	GLuint vaoID;

	vector<vec3> vertices;
	vector<vec2> texCoords;
	vector<unsigned int> indices;

	TextureManager* textureManager;

	void GenerateVertices();
	void GenerateBuffers();

public:
	Terrain(int width, int height, int scaleDown, string heightMapPath, string normalMapPath);

	void Initialize();
	void Update(double deltaTime);
	void Render(mat4 projection, mat4 view);
};