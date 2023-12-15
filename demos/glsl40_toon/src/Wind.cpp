#include "Wind.h"
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>


Wind::Wind(vec3 position, vec3 scale) : Entity (position, vec3(0), scale)
{
	this->totalParticles = 10;
	this->particleDistance = 2;
}

float Wind::GetLoopingRotation(float animationKey)
{
	float a = 270;
	if (animationKey >= 0.5)
	{
		return animationKey * 2 * a;
	}

	animationKey = animationKey * 2 - 1;

	a = 1280 / 3.0f;
	float b = -640;
	float c = 640 / 3.0f;

	return a * animationKey * animationKey * animationKey +
		b * animationKey * animationKey +
		c * animationKey;
}

float Wind::GetBreezeRotation(float animationKey)
{
	return 0;
}

#pragma region GenerateFunctions

void Wind::GenerateVertices()
{
	for (int i = 0; i < totalParticles; i++)
	{
		vertices.push_back(vec3(-0.5 + i * particleDistance, -0.5, 0));
		vertices.push_back(vec3(-0.5 + i * particleDistance, 0.5, 0));
		vertices.push_back(vec3(0.5 + i * particleDistance, 0.5, 0));
		vertices.push_back(vec3(0.5 + i * particleDistance, -0.5, 0));

		indices.push_back(0 + i * 4);
		indices.push_back(1 + i * 4);
		indices.push_back(2 + i * 4);
		indices.push_back(3 + i * 4);
		indices.push_back(primitiveRestartIndex);
	}
}

void Wind::GenerateBuffers()
{
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);

	unsigned int handle[2];
	glGenBuffers(2, handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), (GLvoid*)&vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), (GLvoid*)&indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

#pragma endregion



#pragma region EntityFunctions

void Wind::Initialize()
{
	try {
		shader.compileShader("shader/Wind.vert", GLSLShader::VERTEX);
		shader.compileShader("shader/Wind.frag", GLSLShader::FRAGMENT);

		shader.link();
		shader.use();
	}
	catch (GLSLProgramException& e) {
		cerr << e.what() << endl;
		system("pause");
		exit(EXIT_FAILURE);
	}
	shader.printActiveAttribs();

	this->primitiveRestartIndex = -1;
	glPrimitiveRestartIndex(primitiveRestartIndex);

	GenerateVertices();
	GenerateBuffers();
}

void Wind::Update(double deltaTime)
{
	UpdateSelfAndChildren(false);
}

void Wind::Render(mat4 projection, mat4 view)
{
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_PRIMITIVE_RESTART_INDEX);

	shader.use();
	mat4 MVPMatrix = projection * view * transform.modelMatrix();

	shader.setUniform("MVP", MVPMatrix);

	glBindVertexArray(vaoID);
	glDrawElements(GL_TRIANGLE_FAN, indices.size(), GL_UNSIGNED_INT, (GLubyte*)NULL);
	glBindVertexArray(0);
}

#pragma endregion
