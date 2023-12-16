#pragma once

#include "glslprogram.h"
#include "Entity.h"
#include <vector>

class Wind : public Entity
{
private:

	float animationStep;
	double animationUpdateCount;
	float totalAnimationDuration;

	int firstWindParticle;
	int lastWindParticle;
	int maxVisibleParticles;

	int totalParticles;
	float particleSpacement;
	int totalSequences;

	vec3 windColor;

	GLuint vaoID;
	GLuint primitiveRestartIndex;
	unsigned int handle[2];

	GLSLProgram shader;
	vector<vec3> vertices;
	vector<unsigned int> indices;

	float GetLoopingRotation(double animationKey);
	float GetBreezeRotation(double animationKey);

	void UpdateParticleIndices();

	void GenerateBuffers();
	void GenerateVertices();
	void GenerateAnimationSequence(int index, float loopingRotation, vec3 displacement);

public:
	bool render;

	Wind(vec3 position, vec3 scale);

	void SetNewSpawn(vec3 position, vec3 angles);

	void Initialize();
	void Update(double deltaTime);
	void Render(mat4 projection, mat4 view);
};