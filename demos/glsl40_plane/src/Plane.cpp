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
	this->size = 0.5f;
	this->circleSteps = 64;
	this->numCircles = 300;
	this->curveSteps = 1000.0f;
	this->primitiveRestartIndex = -1; // numero magico
	this->window = window;
	this->wireframe = false;
	this->camSpeed = 20.0f;
	this->planePos = vec3(0.0f, 0.0f, 2.5f);
	this->hashDimension = 10;
	this->maxCoords = 100.0f;
	this->camPos = vec3(maxCoords / 2.0f, maxCoords / 2.0f, -75.0f);
	this->numControlPoints = 200;
	this->LODFactor = 1;
}

void Plane::init(){
	srand(time(0));

	// init matrices
	modelMatrix = glm::mat4(1.0f);
	UpdateViewMatrix();
	projectionMatrix = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 200.0f);

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

	PrintParameters();
	PrintCommands();

	glPrimitiveRestartIndex(primitiveRestartIndex);
	genCurve();
	genCircles();

	glGenVertexArrays(2, vaoIDs);
	genCirclesBuffers();
	genCurveBuffers();
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
	float radius = size;
	float increment = 6.28 / (float)circleSteps;
	float ang = 0.0;

	circlesVertices = vector<vec3>();
	circlesColors = vector<vec3>();
	circlesIndices = vector<unsigned int>();

	for (int i = 0; i < numCircles; i++)
	{
		float xPos = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / maxCoords));
		float yPos = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / maxCoords));

		vec2 center = vec2(xPos, yPos);
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
	Curva curve = Curva(curveSteps);

	for (int i = 0; i < numControlPoints; i++)
	{
		float xPos = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / maxCoords));
		float yPos = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / maxCoords));

		vec3 point = vec3(xPos, yPos, 0.0f);
		curve.addControlPoint(point);
	}

	unsigned int i = 0;
	curveVertices = std::vector<vec3>();
	curveColors = std::vector<vec3>();
	curveIndices = std::vector<unsigned int>();

	for (vec3 &point : curve.curvePoints)
	{
		curveVertices.push_back(point);
		curveColors.push_back(vec3(1.0f / (point.x * point.y), point.x * point.x / (point.y * point.y), point.y * point.y / (point.x * point.x)));
		curveIndices.push_back(i);
		i++;
	}

	GenerateHashTable();
}

void Plane::CheckAllCollisions()
{
	double start = glfwGetTime();
	double startColoring = 0.0;
	double timeSpentColoring = 0.0;

	for (int i = 0; i < numCircles; i++)
	{
		int centerIndex = i * (circleSteps + 1);
		vec3 centerPos = circlesVertices[centerIndex];

		for (size_t j = 0; j < curveVertices.size() - 1; j++)
		{
			if (CollidesWithCurve(centerPos, curveVertices[j], curveVertices[j + 1]))
			{
				startColoring = glfwGetTime();
				PaintCollidedCircle(centerIndex);
				timeSpentColoring += glfwGetTime() - startColoring;
				break;
			}			
		}
	}
	cout << endl << "Tempo checando colisoes por forca bruta: " << glfwGetTime() - start - timeSpentColoring << endl;
	genCirclesBuffers();
}

void Plane::CheckHashCollisions()
{
	double start = glfwGetTime();
	double startColoring = 0.0;
	double timeSpentColoring = 0.0;

	for (int i = 0; i < numCircles; i++)
	{
		int centerIndex = i * (circleSteps + 1);
		vec3 centerPos = circlesVertices[centerIndex];

		int index = HashFunction(centerPos);
		vec3 hashInfo = hashPivots[index];

		for (int j = hashInfo.x; j < hashInfo.x + hashInfo.z; j++)
		{
			int curveIndex = hashTable[j];

			if (curveIndex != 0)
			{
				if (CollidesWithCurve(centerPos, curveVertices[curveIndex - 1], curveVertices[curveIndex]))
				{
					startColoring = glfwGetTime();
					PaintCollidedCircle(centerIndex);
					timeSpentColoring += glfwGetTime() - startColoring;
					break;
				}
			}
			if (curveIndex != curveVertices.size() - 1)
			{
				if (CollidesWithCurve(centerPos, curveVertices[curveIndex], curveVertices[curveIndex + 1]))
				{
					startColoring = glfwGetTime();
					PaintCollidedCircle(centerIndex);
					timeSpentColoring += glfwGetTime() - startColoring;
					break;
				}
			}
		}
	}
	cout << endl << "Tempo checando colisoes por hash table: " << glfwGetTime() - start - timeSpentColoring << endl;
	genCirclesBuffers();
}

void Plane::GenerateHashTable()
{
	double start = glfwGetTime();

	std::vector<int> objectsIndex;
	std::vector<int> usedIndex(hashDimension*hashDimension, 0);

	for (size_t i = 0; i < curveVertices.size(); i++)
	{
		int index = HashFunction(curveVertices[i]);
		objectsIndex.push_back(index);
		usedIndex[index] += 1;
	}

	int count = 0;
	hashPivots = std::vector<glm::ivec3>(hashDimension * hashDimension);

	for (size_t i = 0; i < usedIndex.size(); i++)
	{
		hashPivots[i].x = count;
		count += usedIndex[i];
		hashPivots[i].y = count;
		hashPivots[i].z = hashPivots[i].y - hashPivots[i].x;
	}

	hashTable = std::vector<int>(curveVertices.size());

	for (size_t i = 0; i < curveVertices.size(); i++)
	{
		hashTable[hashPivots[objectsIndex[i]].y - 1] = i;
		hashPivots[objectsIndex[i]].y -= 1;
	}

	cout << endl << "Tempo de geracao da tabela Hash: " << glfwGetTime() - start << endl;
}

int Plane::HashFunction(vec3 point)
{
	int factor = maxCoords / hashDimension;
	return (int) (point.x / factor) + hashDimension * (int) (point.y / factor);
}

bool Plane::CollidesWithCurve(vec3 centerPos, vec3 point1, vec3 point2)
{
	vec3 threshold = vec3(0.000030517578125f, 0.000030517578125f, 0.000030517578125f); // 1 / 2^15

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

void Plane::PaintCollidedCircle(int centerIndex)
{
	for (int k = centerIndex; k <= centerIndex + circleSteps; k++)
	{
		circlesColors[k] = vec3(1.0f, 0.0f, 0.0f);
	}
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
		return;
	}
	if (glfwGetKeyOnce(window, '2'))
	{
		CheckAllCollisions();
		return;
	}
	if (glfwGetKeyOnce(window, '3'))
	{
		CheckHashCollisions();
		return;
	}
	if (glfwGetKeyOnce(window, 'R'))
	{
		genCircles();
		genCirclesBuffers();
		return;
	}
	if (glfwGetKeyOnce(window, 'T'))
	{
		numCircles += 50;
		genCircles();
		genCirclesBuffers();
		return;
	}
	if (glfwGetKeyOnce(window, 'G'))
	{
		numCircles -= 50;
		if (numCircles < 50)
		{
			numCircles = 50;
		}
		genCircles();
		genCirclesBuffers();
		return;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camPos.x += camSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camPos.x -= camSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camPos.y += camSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camPos.y -= camSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		camPos.z += camSpeed * deltaTime * 2;
		CheckLODChange();
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		camPos.z -= camSpeed * deltaTime * 2;
		CheckLODChange();
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

void Plane::CheckLODChange()
{
	float levelSize = maxCoords/2.5f;

	int power = abs(camPos.z) / levelSize;
	int factor = pow(2, power);

	if (factor != LODFactor)
	{
		LODFactor = factor;
		UpdateLOD(factor);
	}
}

void Plane::UpdateLOD(int factor)
{
	if (circleSteps / factor < 4)
	{
		return;
	}

	circlesIndices = std::vector<unsigned int>();
	curveIndices = std::vector<unsigned int>();

	for (int i = 0; i < numCircles; i++)
	{
		int baseIndex = i * (circleSteps + 1);
		circlesIndices.push_back(baseIndex);

		for (int j = 0; j < circleSteps; j += factor)
		{
			circlesIndices.push_back(baseIndex + j + 1);
		}
		circlesIndices.push_back(baseIndex + 1);
		circlesIndices.push_back(primitiveRestartIndex);
	}
	for (size_t i = 0; i < curveVertices.size(); i += factor)
	{
		curveIndices.push_back(i);
	}
	genCurveBuffers();
	genCirclesBuffers();
}

void Plane::PrintParameters()
{
	cout << endl << "Principais parametros inicializados" << endl;

	cout << endl << "Parametros dos circulos" << endl;
	cout << "Raio: " << size << endl;
	cout << "Divisoes maximas: " << circleSteps << endl;
	cout << "Total de circulos: " << numCircles << endl;

	cout << endl << "Parametros da curva" << endl;
	cout << "Pontos de controle: " << numControlPoints << endl;
	cout << "Divisoes: " << curveSteps << endl;

	cout << endl << "Parametros do hash" << endl;
	cout << "Dimensao: " << hashDimension << endl;
	cout << "Tamanho de cada celula: " << maxCoords / hashDimension << endl;
}

void Plane::PrintCommands()
{
	cout << endl << "Q: Sobe a camera" << endl;
	cout << "E: Desce a camera" << endl;
	cout << endl << "W: Anda pra frente" << endl;
	cout << "S: Anda pra tras" << endl;
	cout << endl << "D: Anda pra direita" << endl;
	cout << "A: Anda pra esquerda" << endl;
	cout << endl << "R: Reinicia pontos" << endl;
	cout << endl << "1: Alterna entre malha e preenchimento" << endl;
	cout << "2: Faz checagem por forca bruta" << endl;
	cout << "3: Faz checagem por hash table" << endl;
}