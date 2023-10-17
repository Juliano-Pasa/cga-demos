#pragma once

#include <GL/glew.h>
#include <GLFW\glfw3.h>
#include <vector>

#include "curva.h"
#include "Scene.h"
#include "glslprogram.h"

class Plane : public Scene
{
public:
	Plane(GLFWwindow* window, int size = 1);

	// mesh virtual functions
	void init();
	void update(double t);
	void render();
	void resize(int, int);

private:
	void genCurve();
	void genCircles();
	void genCirclesBuffers();
	void genCurveBuffers();
	void CheckAllCollisions();
	void CheckHashCollisions();
	void UpdateViewMatrix();
	void processInput(double deltaTime);

	// Hash
	void GenerateHashTable();
	int HashFunction(vec3 point);
	int hashDimension;
	float hashSize;
	std::vector<glm::ivec3> hashPivots;
	std::vector<vec3> hashTable;


	bool CollidesWithCurve(vec3 centerPos, vec3 point1, vec3 point2);

	GLuint vaoIDs[2];
	std::vector<vec3> normals;

	std::vector<vec3> circlesVertices;
	std::vector<vec3> circlesColors;
	std::vector<unsigned int> circlesIndices;

	std::vector<vec3> curveVertices;
	std::vector<vec3> curveColors;
	std::vector<unsigned int> curveIndices;

	GLSLProgram shader;
	GLFWwindow* window;

	glm::mat4 modelMatrix;
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	glm::mat4 modelViewProjectionMatrix;
	glm::mat4 modelViewMatrix;

	vec3 planePos;
	vec3 camPos;

	float camSpeed;
	float size;

	int circleSteps;
	int numCircles;
	GLuint primitiveRestartIndex;

	bool wireframe;
};