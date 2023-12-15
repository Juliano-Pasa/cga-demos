#pragma once

#include "glslprogram.h"
#include "Entity.h"
#include <vector>

class Wind : public Entity
{
private:
	void GenerateVertices();
	void GenerateBuffers();

	float totalAnimationTime;
	float currentAnimationKey;

	int firstWindParticle;
	int lastWindParticle;

	int totalParticles;
	float particleDistance;

	GLuint vaoID;
	GLuint primitiveRestartIndex;
	vector<vec3> vertices;
	vector<unsigned int> indices;
	GLSLProgram shader;

	float GetLoopingRotation(float animationKey);
	float GetBreezeRotation(float animationKey);

public:
	Wind(vec3 position, vec3 scale);

	void Initialize();
	void Update(double deltaTime);
	void Render(mat4 projection, mat4 view);
};