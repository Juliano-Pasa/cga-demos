#include "Plane.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <cmath>

using namespace std;

//add to glfwGetKey that gets the pressed key only once (not several times)
char keyOnce[GLFW_KEY_LAST + 1];
#define glfwGetKeyOnce(WINDOW, KEY)             \
    (glfwGetKey(WINDOW, KEY) ?              \
     (keyOnce[KEY] ? false : (keyOnce[KEY] = true)) :   \
     (keyOnce[KEY] = false))

Plane::Plane(GLFWwindow* window, int size){
	this->size = 0.2f;
	this->numCircles = 100;
	this->primitiveRestartIndex = -1; // numero magico
	this->window = window;
	this->wireframe = false;
	this->camSpeed = 10.0f;
	this->camPos = vec3(25.0f, 25, -30.0f);
	this->planePos = vec3(0.0f, 0.0f, 2.5f);
	this->hashDimension = 10;
	this->hashSize = 50.0f;
}

void Plane::init(){
	srand(time(0));

	glPrimitiveRestartIndex(primitiveRestartIndex);
	genCurve();
	genCircles();

	glGenVertexArrays(2, vaoIDs);
	genCirclesBuffers();
	genCurveBuffers();

	// init matrices
	modelMatrix = glm::mat4(1.0f);
	UpdateViewMatrix();
	projectionMatrix = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 100.0f);

	// load shaders
	try {
		shader.compileShader("shader/glsl40_plane.vert");
		shader.compileShader("shader/glsl40_plane.frag");

		shader.link();
		shader.use();
	}
	catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		system("pause");
		exit(EXIT_FAILURE);
	}
	shader.printActiveAttribs();
}

void Plane::update(double deltaTime){

	processInput(deltaTime);

	//// matrices setup
	modelMatrix = mat4(1.0f); // identity
	modelMatrix = glm::translate(modelMatrix, planePos); // translate back
	modelViewMatrix = viewMatrix * modelMatrix;
	modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

	// set var MVP on the shader
	shader.setUniform("MVP", modelViewProjectionMatrix); //ModelViewProjection
}

void Plane::render(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(vaoIDs[0]);
	glDrawElements(GL_TRIANGLE_FAN, circlesIndices.size(), GL_UNSIGNED_INT, (GLubyte *)NULL);

	glBindVertexArray(vaoIDs[1]);
	glDrawElements(GL_LINE_STRIP, curveIndices.size(), GL_UNSIGNED_INT, (GLubyte*)NULL);

	glBindVertexArray(0);
}

void Plane::genCirclesBuffers(){

	glBindVertexArray(vaoIDs[0]);

	unsigned int handle[3];
	glGenBuffers(3, handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, circlesVertices.size() * sizeof(vec3), (GLvoid*)&circlesVertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL); // Vertex position -> layout 0 in the VS
	glEnableVertexAttribArray(0);  

	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, circlesColors.size() * sizeof(vec3), (GLvoid*)&circlesColors[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL); // Vertex color -> layout 1 in the VS
	glEnableVertexAttribArray(1);  

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, circlesIndices.size() * sizeof(int), (GLvoid*)&circlesIndices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void Plane::genCurveBuffers() {

	glBindVertexArray(vaoIDs[1]);

	unsigned int handle[3];
	glGenBuffers(3, handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, curveVertices.size() * sizeof(vec3), (GLvoid*)&curveVertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL); // Vertex position -> layout 0 in the VS
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, curveColors.size() * sizeof(vec3), (GLvoid*)&curveColors[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL); // Vertex color -> layout 1 in the VS
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, curveIndices.size() * sizeof(int), (GLvoid*)&curveIndices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void Plane::resize(int x, int y){

}

void Plane::genCircles()
{
	circleSteps = 128;
	float radius = size;
	float increment = 6.28 / (float)circleSteps;
	float ang = 0.0;

	circlesVertices = vector<vec3>();
	circlesColors = vector<vec3>();
	circlesIndices = vector<unsigned int>();

	for (int i = 0; i < numCircles; i++)
	{
		vec2 center = vec2(rand() % 50, rand() % 50);
		int baseIndex = i * (circleSteps + 1);

		circlesVertices.push_back(vec3(center, 0.0f));
		circlesColors.push_back(vec3(1.0f, 0.0f, 0.7f));
		circlesIndices.push_back(baseIndex);

		for (int j = 0; j < circleSteps; j++)
		{
			double cossine = cos(ang);
			double sine = sin(ang);
			circlesVertices.push_back(vec3(cossine * radius + center.x, sine * radius + center.y, 0.0f));
			circlesColors.push_back(vec3(cossine, sine, 0.7f));
			circlesIndices.push_back(baseIndex + j + 1);
			ang += increment;
		}

		circlesIndices.push_back(baseIndex + 1);
		circlesIndices.push_back(primitiveRestartIndex);
	}
}

void Plane::genCurve()
{
	int totalPoints = 100;
	float curveSteps = 100.0f;
	Curva curve = Curva(curveSteps);

	for (int i = 0; i < totalPoints; i++)
	{
		vec3 point = vec3((rand() % totalPoints/2), (rand() % totalPoints/2), 0.0f);
		curve.addControlPoint(point);
	}

	unsigned int i = 0;
	for (vec3 &point : curve.curvePoints)
	{
		curveVertices.push_back(point);
		curveColors.push_back(point / (float)(totalPoints/2));
		curveIndices.push_back(i);
		i++;
	}
}

void Plane::CheckAllCollisions()
{
	for (int i = 0; i < numCircles; i++)
	{
		int centerIndex = i * (circleSteps + 1);
		vec3 centerPos = circlesVertices[centerIndex];

		for (size_t j = 0; j < curveVertices.size() - 1; j++)
		{
			if (!CollidesWithCurve(centerPos, curveVertices[j], curveVertices[j+1]))
			{
				continue;
			}

			for (int k = centerIndex; k <= centerIndex + circleSteps; k++)
			{
				circlesColors[k] = vec3(0.0f, 0.0f, 0.0f);
			}
		}
	}
	genCirclesBuffers();
}

void Plane::CheckHashCollisions()
{

}

void Plane::GenerateHashTable()
{

}

int Plane::HashFunction(vec3 point)
{
	int factor = hashSize / hashDimension;
	return point.x / factor + hashDimension * point.y / factor;
}


bool Plane::CollidesWithCurve(vec3 centerPos, vec3 point1, vec3 point2)
{
	vec3 threshold = vec3(0.001f, 0.001f, 0.001f);

	vec3 vecDir = point2 - point1 + threshold;
	vecDir = glm::normalize(vecDir);
	float len = glm::length(vecDir);

	vec3 vecLine = centerPos - point1;
	float scalar = glm::dot(vecLine, vecDir);

	if (scalar < 0 || scalar > len)
	{
		return false;
	}

	vec3 projection = point1 + vecDir * scalar;
	float distance = glm::distance(projection, centerPos);

	if (distance > size)
	{
		return false;
	}

	return true;
	
}

void Plane::processInput(double deltaTime)
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
	}
	if (glfwGetKeyOnce(window, '2'))
	{
		CheckAllCollisions();
	}
	if (glfwGetKeyOnce(window, '3'))
	{
		genCircles();
		genCirclesBuffers();
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camPos[0] += camSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camPos[0] -= camSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camPos[1] += camSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camPos[1] -= camSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		camPos[2] += camSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		camPos[2] -= camSpeed * deltaTime;
	}
	UpdateViewMatrix();
}

void Plane::UpdateViewMatrix()
{
	vec3 centerPos = camPos;
	centerPos.z += 1.0f;
	viewMatrix = glm::lookAt(
		camPos, //eye
		centerPos, //center
		vec3(0.0f, 1.0f, 0.0f)); //up
}