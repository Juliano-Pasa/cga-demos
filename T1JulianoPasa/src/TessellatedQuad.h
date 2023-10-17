#pragma once

#include <GL/glew.h>

#include <GLFW\glfw3.h>
#include "Scene.h"
#include <vector>
#include "glslprogram.h"

class TessellatedQuad : public Scene
{
public:
	TessellatedQuad(GLFWwindow* window, int size);

	// mesh virtual functions
	void init();
	void update(double t);
	void render();
	void resize(int, int);

	float elapsedTime;

private:
	void genPlane();
	void genBuffers();
	void processInput(double deltaTime);
	void GenerateLights();
	void UpdateViewMatrix();
	void InitializeColors();
	void GenerateWaveDirections(int totalWaves);

	GLuint vaoID;
	int size;
	int width;
	int height;
	int totalWaves;
	std::vector<vec3> vertices;
	std::vector<unsigned int> indices;

	GLSLProgram shader;
	GLFWwindow* window;

	glm::mat4 modelMatrix;
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	glm::mat4 modelViewProjectionMatrix;
	glm::mat4 modelViewMatrix;
	glm::mat3 normalMatrix;
	glm::mat4 rotationMatrix;

	std::vector<vec2> waveDirections;
	std::vector<vec3> lightsPositions;
	std::vector<vec3> lightColors;

	vec3 planePos;
	vec3 camPos;
	vec3 lightColor;

	float camSpeed;
	float lightsRotationSpeed;
	float maxTesselationDistance;

	bool wireframe;
	bool updateTime;

	int tcsMode;
	int lightColorId;
	int centerPatchTessLevel;
};