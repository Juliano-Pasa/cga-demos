#include <iostream>
#include <sstream>
#include <fstream>
#include "Duck.h"
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>


Duck::Duck(vec3 position, vec3 scale, WorldLight* worldLight, Camera* camera, InputManager* inputManager, bool playerControled) : Entity(position, vec3(0), scale)
{
	vaoID = 0;
	this->worldLight = worldLight;
	this->camera = camera;
	this->inputManager = inputManager;
	this->playerControled = playerControled;

	this->steeringForce = vec3(0);
	this->resultingForce = vec3(0);
	this->maxForce = 1000.0f;
	this->movementStrength = 100.0f;

	this->baseSpeed = 250.0f;
	this->sprintSpeed = 350.0f;
	this->maxSpeed = baseSpeed;
	this->currentSpeed = vec3(0);

	this->holdRotation = false;

	this->mass = 1.0f;
}

#pragma region EntityFunctions

void Duck::Initialize()
{
	try {
		shader.compileShader("shader/Duck.vert", GLSLShader::VERTEX);
		shader.compileShader("shader/Duck.geom", GLSLShader::GEOMETRY);
		shader.compileShader("shader/Duck.frag", GLSLShader::FRAGMENT);

		shader.link();
		shader.use();
	}
	catch (GLSLProgramException& e) {
		cerr << e.what() << endl;
		system("pause");
		exit(EXIT_FAILURE);
	}
	shader.printActiveAttribs();

	glEnable(GL_PRIMITIVE_RESTART);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	primitiveRestartIndex = -1;
	glPrimitiveRestartIndex(primitiveRestartIndex);

	GenerateModel();
	GenerateBuffers();
}


void Duck::Update(double deltaTime)
{
	shader.use();
	shader.setUniform("lightPos", worldLight->GetPosition());

	if (playerControled)
	{
		ReadMouseInputs();
		ReadKeyboardInputs((float) deltaTime);
		ApplyForces((float) deltaTime);
	}
	
	UpdateSelfAndChildren(false);
}

void Duck::Render(mat4 projection, mat4 view)
{
	shader.use();
	mat4 VPMatrix = projection * view;

	shader.setUniform("VP", VPMatrix);
	shader.setUniform("M", transform.modelMatrix());

	glBindVertexArray(vaoID);
	glDrawElements(GL_TRIANGLE_FAN, indices.size(), GL_UNSIGNED_INT, (GLubyte*)NULL);
	glBindVertexArray(0);
}

#pragma endregion

#pragma region GenerateFunctions

void Duck::GenerateModel()
{
	vertices.push_back(vec3(-0.5, 0.5, -0.5));
	vertices.push_back(vec3(0.5, 0.5, -0.5));
	vertices.push_back(vec3(0.5, 0.5, 0.5));
	vertices.push_back(vec3(-0.5, 0.5, 0.5));

	vertices.push_back(vec3(-0.5, -0.5, 0.5));
	vertices.push_back(vec3(-0.5, -0.5, -0.5));
	vertices.push_back(vec3(0.5, -0.5, -0.5));
	vertices.push_back(vec3(0.5, -0.5, 0.5));

	colors.push_back(vec3(0, 0, 0));
	colors.push_back(vec3(0, 0, 1));
	colors.push_back(vec3(0, 1, 0));
	colors.push_back(vec3(0, 1, 1));
	colors.push_back(vec3(1, 0, 0));
	colors.push_back(vec3(1, 0, 1));
	colors.push_back(vec3(1, 1, 0));
	colors.push_back(vec3(1, 1, 1));

	indices.push_back(0);
	indices.push_back(3);
	indices.push_back(1);
	indices.push_back(5);
	indices.push_back(3);

	indices.push_back(primitiveRestartIndex);

	indices.push_back(4);
	indices.push_back(5);
	indices.push_back(7);
	indices.push_back(3);
	indices.push_back(5);

	indices.push_back(primitiveRestartIndex);

	indices.push_back(2);
	indices.push_back(1);
	indices.push_back(3);
	indices.push_back(7);
	indices.push_back(1);

	indices.push_back(primitiveRestartIndex);

	indices.push_back(6);
	indices.push_back(7);
	indices.push_back(5);
	indices.push_back(1);
	indices.push_back(7);
}

void Duck::GenerateBuffers()
{
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);

	unsigned int handle[3];
	glGenBuffers(3, handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), (GLvoid*)&vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), (GLvoid*)&colors[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), (GLvoid*)&indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}


#pragma endregion

#pragma region InputFunctions

void Duck::ReadKeyboardInputs(float deltaTime)
{
	if (inputManager->GetIsKeyDown(GLFW_KEY_W))
	{
		resultingForce += movementStrength * orientation;
	}
	if (inputManager->GetIsKeyDown(GLFW_KEY_S))
	{
		resultingForce -= movementStrength * orientation;
	}
	if (inputManager->GetIsKeyDown(GLFW_KEY_A))
	{
		resultingForce -= movementStrength * sideOrientation;
	}
	if (inputManager->GetIsKeyDown(GLFW_KEY_D))
	{
		resultingForce += movementStrength * sideOrientation;
	}

	if (inputManager->GetIsKeyDown(GLFW_KEY_LEFT_SHIFT))
	{
		maxSpeed = sprintSpeed;
	}
	if (!inputManager->GetIsKeyDown(GLFW_KEY_LEFT_SHIFT))
	{
		maxSpeed = baseSpeed;
	}

	if (inputManager->GetIsKeyDown(GLFW_KEY_LEFT_CONTROL))
	{
		holdRotation = true;
		firstMouseMove = true;
	}
	if (!inputManager->GetIsKeyDown(GLFW_KEY_LEFT_CONTROL))
	{
		holdRotation = false;
	}
}

void Duck::ReadMouseInputs()
{
	if (holdRotation)
	{
		return;
	}

	dvec2 mouseCoords = inputManager->GetMouseCoords();

	if (firstMouseMove)
	{
		lastMouseCoords = mouseCoords;
		firstMouseMove = false;
		return;
	}

	dvec2 delta = mouseCoords - lastMouseCoords;
	lastMouseCoords = mouseCoords;

	if (delta.x * delta.x < 0.001)
	{
		return;
	}

	vec3 currentAngles = transform.angles();
	currentAngles.y -= glm::radians((float)delta.x * camera->sensitivity);

	transform.angles(currentAngles);

	CalculateOrientation();
}

#pragma endregion

void Duck::ApplyForces(float deltaTime)
{
	resultingForce = TruncateMagnitude(resultingForce, maxForce);

	vec3 acceleration = resultingForce / mass;
	currentSpeed += (acceleration * deltaTime);

	currentSpeed = TruncateMagnitude(currentSpeed, maxSpeed);
	transform.position(transform.position() + currentSpeed * deltaTime);

	if (resultingForce.length() > 1)
	{
		resultingForce -= resultingForce / 10.0f;
		return;
	}

	resultingForce = vec3(0);
	if (currentSpeed.length() > 1)
	{
		currentSpeed -= currentSpeed / 10.0f;
		return;
	}
	currentSpeed = vec3(0);
}

void Duck::CalculateOrientation()
{
	vec3 newOrientation = glm::rotateY(vec3(1.0f, 0.0f, 0.0f), transform.angles().y);
	orientation = glm::normalize(newOrientation);

	sideOrientation = glm::rotateY(orientation, glm::radians(-90.0f));
}

vec3 Duck::TruncateMagnitude(vec3 vec, float maxMagnitude)
{	
	if (glm::length(vec) <= maxMagnitude)
	{
		return vec;
	}

	return glm::normalize(vec) * maxMagnitude;
}
