#pragma once

#include "Entity.h"
#include "Camera.h"
#include "WorldLight.h"
#include "TextureManager.h"
#include <string>
#include <vector>

using namespace std;

class Terrain : public Entity
{
private:
	int width;
	int height;
	int totalPatches;
	int patchWidth;

	GLuint vaoID;

	vector<vec3> vertices;
	vector<vec3> verticesColors;
	vector<unsigned int> indices;

	TextureManager* textureManager;
	GLSLProgram shader;

	Camera* camera;

	void GenerateVertices();
	void GenerateBuffers();

	vector<vector<float>> ReadHeightMap(string mapPath);
	vector<vector<vec3>> ReadNormalMap(string mapPath);

public:
	Terrain(int width, int height, int totalPatches, Camera* camera, string heightMapPath, string normalMapPath);

	void Initialize();
	void Update(double deltaTime);
	void Render(mat4 projection, mat4 view);

	WorldLight* worldLight;
};