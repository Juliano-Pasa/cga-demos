#include "TessellatedQuad.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

using namespace std;

//add to glfwGetKey that gets the pressed key only once (not several times)
char keyOnce[GLFW_KEY_LAST + 1];
#define glfwGetKeyOnce(WINDOW, KEY)             \
    (glfwGetKey(WINDOW, KEY) ?              \
     (keyOnce[KEY] ? false : (keyOnce[KEY] = true)) :   \
     (keyOnce[KEY] = false))

TessellatedQuad::TessellatedQuad(GLFWwindow* window, int size)
{
	this->size = size;
	this->window = window;
	InitializeColors();
	planePos = vec3(0.0f, 0.0f, 2.5f);
}

void TessellatedQuad::init()
{
	// init parameters
	this->size = 5;
	this->width = 20;
	this->height = 20;
	this->camSpeed = 20.0f;
	this->elapsedTime = 0.0f;
	this->totalWaves = 16;
	this->lightsRotationSpeed = 0.5f;
	this->wireframe = true;
	this->updateTime = true;
	this->tcsMode = 0;
	this->centerPatchTessLevel = 8;
	this->camPos = vec3(0.0f, 20.0f, 0.0f);
	this->lightsRotationSpeed = 1.0f;
	this->maxTesselationDistance = 50.0f;
	this->lightColor = lightColors[0];
	this->lightColorId = 0;

	int centerPatch = height * width / 2 + width / 2;

	genPlane();
	genBuffers();
	GenerateLights();
	GenerateWaveDirections(totalWaves);

	// init matrices
	UpdateViewMatrix();
	rotationMatrix = glm::rotate(lightsRotationSpeed, vec3(0, 1, 0));
	modelMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::perspective(glm::radians(75.0f), 1.0f, 0.1f, 500.0f);

	// load shaders
	try {
		shader.compileShader("shader/glsl40_basic_tess.vert", GLSLShader::VERTEX);
		shader.compileShader("shader/glsl40_basic_tess.tcs", GLSLShader::TESS_CONTROL);
		shader.compileShader("shader/glsl40_basic_tess.tes", GLSLShader::TESS_EVALUATION);
		shader.compileShader("shader/glsl40_basic_tess.geom", GLSLShader::GEOMETRY);
		shader.compileShader("shader/glsl40_basic_tess.frag", GLSLShader::FRAGMENT);

		shader.link();
		shader.use();
	}
	catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		system("pause");
		exit(EXIT_FAILURE);
	}
	shader.printActiveAttribs();

	// Uniforms usados no TCS
	shader.setUniform("maxTess", 64);
	shader.setUniform("minTess", 4);
	shader.setUniform("centerPatch", centerPatch);
	shader.setUniform("surroundingPatches", vec4(centerPatch + 1, centerPatch + width, centerPatch - 1, centerPatch - width));

	// Uniforms usados no TES
	shader.setUniform("waveSpeed", 1.4f);
	shader.setUniform("waveAmplitudeDecay", 0.7f);
	shader.setUniform("waveFrequencyIncrease", 1.18f);
	shader.setUniform("totalWaves", totalWaves);
	shader.setUniform("wavesDirections", waveDirections.data());

	// Uniforms usados no FS
	shader.setUniform("oceanColor", vec3(0.16f, 0.39f, 0.92f));
}

void TessellatedQuad::update(double deltaTime)
{
	processInput(deltaTime);
	if (updateTime)
	{
		elapsedTime += deltaTime;
	}

	mat4 rotation0 = glm::rotate((float)deltaTime * lightsRotationSpeed, vec3(0, 1, 0));
	mat4 rotation1 = glm::rotate((float)deltaTime * lightsRotationSpeed * 1.13f, vec3(0, 1, 0));
	lightsPositions[0] = vec3(rotation0 * vec4(lightsPositions[0], 1.0));
	lightsPositions[1] = vec3(rotation1 * vec4(lightsPositions[1], 1.0));

	//// matrices setup
	modelMatrix = mat4(1.0f); // identity
	modelMatrix = glm::translate(modelMatrix, planePos); // translate back
	modelViewMatrix = viewMatrix * modelMatrix;
	modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;
	
	normalMatrix = glm::mat3(glm::inverse(glm::transpose(modelViewMatrix)));

	// set var MVP on the shader
	shader.setUniform("MVP", modelViewProjectionMatrix); //ModelViewProjection
	shader.setUniform("NM", normalMatrix);

	shader.setUniform("camPos", camPos);
	shader.setUniform("lightPos0", lightsPositions[0]);
	shader.setUniform("lightPos1", lightsPositions[1]);
	shader.setUniform("maxDist", maxTesselationDistance);
	shader.setUniform("elapsedTime", elapsedTime);
	shader.setUniform("tcsMode", tcsMode);
	shader.setUniform("lightColor", lightColor);
	shader.setUniform("centerPatchTessLevel", centerPatchTessLevel);
}

void TessellatedQuad::processInput(double deltaTime)
{
	if (glfwGetKeyOnce(window, '1')) 
	{
		wireframe = !wireframe;
		if (wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		return;
	}
	if (glfwGetKeyOnce(window, '2')) 
	{
		updateTime = !updateTime;
		return;
	}
	if (glfwGetKeyOnce(window, '3'))
	{
		tcsMode = (tcsMode + 1) % 3;
		return;
	}
	if (glfwGetKeyOnce(window, '4'))
	{
		lightColorId = (lightColorId + 1) % 8;
		lightColor = lightColors[lightColorId];
		return;
	}
	if (glfwGetKeyOnce(window, 'R'))
	{
		maxTesselationDistance *= 1.5f;
		return;
	}
	if (glfwGetKeyOnce(window, 'F'))
	{
		maxTesselationDistance /= 1.5f;
		return;
	}
	if (glfwGetKeyOnce(window, 'T'))
	{
		centerPatchTessLevel *= 2;
		return;
	}
	if (glfwGetKeyOnce(window, 'G'))
	{
		centerPatchTessLevel /= 2;
		if (centerPatchTessLevel < 2)
		{
			centerPatchTessLevel = 2;
		}
		return;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camPos[0] += camSpeed*deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camPos[0] -= camSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camPos[2] += camSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camPos[2] -= camSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		camPos[1] += camSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		camPos[1] -= camSpeed * deltaTime;
	}
	UpdateViewMatrix();
}

void TessellatedQuad::UpdateViewMatrix()
{
	vec3 centerPos = camPos;
	centerPos.y -= 1.0f;
	centerPos.z += 1.0f;
	viewMatrix = glm::lookAt(
		camPos, //eye
		centerPos, //center
		vec3(0.0f, 0.0f, 1.0f)); //up
}

void TessellatedQuad::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(vaoID);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawElements(GL_PATCHES, indices.size(), GL_UNSIGNED_INT, (GLubyte *)NULL);
	glBindVertexArray(0);
}

void TessellatedQuad::genBuffers()
{
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);

	unsigned int handle[2];
	glGenBuffers(2, handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), (GLvoid*)&vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
	glEnableVertexAttribArray(0);  // Vertex position -> layout 0 in the VS

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), (GLvoid*)&indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void TessellatedQuad::resize(int x, int y)
{

}

void TessellatedQuad::genPlane()
{
	int startingX = -size * width;
	int startingZ = -size * height;

	for (int i = 0; i < height+1; i++)
	{
		for (int j = 0; j < width+1; j++)
		{
			vertices.push_back(vec3((float)startingX + size * j * 2, 0.0f, (float)startingZ + size * i * 2));
		}
	}

	for (int patch = 0; patch < width * height; patch++)
	{
		int startingPoint = (int)(patch / width);
		startingPoint += patch;

		indices.push_back(startingPoint);
		indices.push_back(startingPoint + 1);
		indices.push_back(startingPoint + width + 2);
		indices.push_back(startingPoint + width + 1);
	}
}

void TessellatedQuad::GenerateWaveDirections(int totalWaves)
{
	for (int i = 0; i < totalWaves; i++)
	{
		float randomAngle = rand();
		waveDirections.push_back(vec2(cos(randomAngle), sin(randomAngle)));
	}
}

void TessellatedQuad::GenerateLights()
{
	lightsPositions.push_back(vec3(0.0f, 20.0f, -100.0f));
	lightsPositions.push_back(vec3(0.0f, 20.0f, 100.0f));
}

void TessellatedQuad::InitializeColors()
{
	lightColors = {
		vec3(1.0f, 1.0f, 1.0f),
		vec3(1.0f, 1.0f, 0.0f),
		vec3(1.0f, 0.0f, 1.0f),
		vec3(0.0f, 1.0f, 1.0f),
		vec3(1.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, 0.0f, 1.0f),
		vec3(0.0f, 0.0f, 0.0f)
	};
}