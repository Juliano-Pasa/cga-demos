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
	unsigned int handle[3];

	vector<vec3> vertices;
	vector<vec3> verticesColors;
	vector<unsigned int> indices;

	Camera* camera;
	GLSLProgram shader;

	void GenerateVertices();
	void GenerateBuffers();

	vec3 dirtColor;
	vector<vec3> grassColors;
	void InitializeGrassColors();

	vec3 goalColor;
	vec3 goalPosition;
	void SetGoalPosition();

	void SetRenderedPatches();
	vector<vector<float>> ReadHeightMap(string mapPath);

public:
	Terrain(int width, int height, int totalPatches, Camera* camera, string heightMapPath, string normalMapPath);

	void Initialize();
	void Update(double deltaTime);
	void Render(mat4 projection, mat4 view);

	vec3 CenterPosition();
	vec3 GoalPosition();
	vector<vec3> GetNearbyVertices(vec3 position);

	WorldLight* worldLight;
};