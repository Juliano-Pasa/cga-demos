#include <iostream>
#include <sstream>
#include <fstream>
#include "Duck.h"
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>


Duck::Duck(vec3 position, vec3 scale, WorldLight* worldLight, Camera* camera, GLFWwindow* window) : Entity(position, vec3(0), scale)
{
	vaoID = 0;
	this->worldLight = worldLight;
	this->camera = camera;
	this->window = window;

	this->baseSpeed = 300.0f;
	this->speed = baseSpeed;
	this->acceleration = 250.0f;
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

	float yAngle = camera->CameraAngles().y;
	transform.angles(vec3(0, yAngle, 0));

	ReadKeyboardInputs((float)deltaTime);
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
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		transform.position(transform.position() + speed * deltaTime * camera->CameraOrientation());
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		transform.position(transform.position() - speed * deltaTime * camera->CameraOrientation());
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		transform.position(transform.position() - speed * deltaTime * glm::normalize(glm::cross(camera->CameraOrientation(), camera->CameraUp())));
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		transform.position(transform.position() + speed * deltaTime * glm::normalize(glm::cross(camera->CameraOrientation(), camera->CameraUp())));
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		speed = baseSpeed + acceleration;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		speed = baseSpeed - acceleration;
	}
}

#pragma endregion
