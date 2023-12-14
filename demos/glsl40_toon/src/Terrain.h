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

	float maxHeight;
	float size;

	float patchWidth;
	float patchHeight;

	GLuint vaoID;

	vector<vec3> vertices;
	vector<vec3> verticesColors;
	vector<vec3> grassColors;
	vector<unsigned int> indices;

	TextureManager* textureManager;
	GLSLProgram shader;

	Camera* camera;

	void GenerateVertices();
	void GenerateBuffers();

	void InitializeGrassColors();

	vector<vector<float>> ReadHeightMap(string mapPath);

public:
	Terrain(int width, int height, int totalPatches, Camera* camera, string heightMapPath, string normalMapPath);

	void Initialize();
	void Update(double deltaTime);
	void Render(mat4 projection, mat4 view);

	vec3 CenterPosition();
	vector<vec3> GetNearbyVertices(vec3 position, int neighbourPatches);

	WorldLight* worldLight;
};