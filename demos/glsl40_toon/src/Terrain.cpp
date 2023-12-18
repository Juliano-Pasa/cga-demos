#include "Terrain.h"
#include <fstream>
#include <time.h>
#include <random>
#include <iostream>
#include <sstream>

Terrain::Terrain(int width, int height, int totalPatches, Camera* camera, string heightMapPath, string normalMapPath) : Entity(vec3(0), vec3(0), vec3(1))
{
	this->width = width;
	this->height = height;
	this->totalPatches = totalPatches;
	this->vaoID = 0;
	this->camera = camera;
}

#pragma region EntityFunctions

void Terrain::Initialize()
{
	InitializeGrassColors();
	GenerateVertices();
	GenerateBuffers();

	try {
		shader.compileShader("shader/TerrainShaders/Terrain.vert", GLSLShader::VERTEX);
		shader.compileShader("shader/TerrainShaders/Terrain.tcs", GLSLShader::TESS_CONTROL);
		shader.compileShader("shader/TerrainShaders/Terrain.tes", GLSLShader::TESS_EVALUATION);
		shader.compileShader("shader/TerrainShaders/Terrain.geom", GLSLShader::GEOMETRY);
		shader.compileShader("shader/TerrainShaders/Terrain.frag", GLSLShader::FRAGMENT);

		shader.link();
		shader.use();
	}
	catch (GLSLProgramException& e) {
		cerr << e.what() << endl;
		system("pause");
		exit(EXIT_FAILURE);
	}
	shader.printActiveAttribs();
}

void Terrain::Update(double deltaTime)
{
	shader.use();
	shader.setUniform("tessLevel", 8);
	shader.setUniform("lodLevelSize", 2000.0f);
	shader.setUniform("patchWidth", patchWidth);
	shader.setUniform("patchHeight", patchHeight);

	shader.setUniform("dirtColor", dirtColor);
	shader.setUniform("camPos", camera->CameraPosition());
	shader.setUniform("lightPos", worldLight->GetPosition());

	SetRenderedPatches();
}

void Terrain::Render(mat4 projection, mat4 view)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	shader.use();
	mat4 VPMatrix = projection * view;

	shader.setUniform("VP", VPMatrix);
	shader.setUniform("M", transform.modelMatrix());

	glBindVertexArray(vaoID);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawElements(GL_PATCHES, indices.size(), GL_UNSIGNED_INT, (GLubyte*)NULL);
	glBindVertexArray(0);
}

#pragma endregion

#pragma region GenerateFunctions

void Terrain::GenerateVertices()
{
	maxHeight = 256.0f;
	size = 64.0f;

	int heightDivisions = (height - 1) / totalPatches;
	int widthDivisions = (width - 1) / totalPatches;

	patchHeight = size;
	patchWidth = size;

	vector<vector<float>> heightMap = ReadHeightMap("..\\..\\resources\\heightMap.csv");

	for (int i = 0; i < totalPatches + 1; i++)
	{
		for (int j = 0; j < totalPatches + 1; j++)
		{
			float heightMapValue = heightMap[i * heightDivisions][j * widthDivisions];
			vertices.push_back(vec3(j * patchWidth, heightMapValue * maxHeight, i * patchHeight));
			verticesColors.push_back(grassColors[int(heightMapValue * 10.0f)]);
		}
	}

	for (int i = 0; i < totalPatches; i++)
	{
		for (int j = 0; j < totalPatches; j++)
		{
			int startingPoint = i * (totalPatches + 1) + j;
			indices.push_back(startingPoint);
			indices.push_back(startingPoint + 1);
			indices.push_back(startingPoint + totalPatches + 2);
			indices.push_back(startingPoint + totalPatches + 1);
		}
	}

	SetGoalPosition();
}

void Terrain::GenerateBuffers()
{
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);

	glGenBuffers(3, handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), (GLvoid*)&vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, verticesColors.size() * sizeof(vec3), (GLvoid*)&verticesColors[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), (GLvoid*)&indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

#pragma endregion

#pragma region ReadMaps

vector<vector<float>> Terrain::ReadHeightMap(string mapPath)
{
	ifstream file;
	file.open(mapPath);

	vector<vector<float>> map;
	vector<float> row;
	string line, word;

	while (getline(file, line))
	{
		row.clear();

		stringstream str(line);

		while (getline(str, word, ','))
		{
			row.push_back(stof(word));
		}
		map.push_back(row);
	}
	file.close();

	return map;
}

#pragma endregion

void Terrain::InitializeGrassColors()
{
	dirtColor = vec3(0.29411f, 0.20784f, 0.0f);
	goalColor = vec3(0.59215f, 0.09019f, 0.90196f);

	grassColors.push_back(vec3(0.23137, 0.31372, 0.02352));
	grassColors.push_back(vec3(0.83137, 0.84705, 0.34901));
	grassColors.push_back(vec3(0.88235, 0.89803, 0.64705));
	grassColors.push_back(vec3(0.43921, 0.55686, 0.02745));
	grassColors.push_back(vec3(0.58039, 0.67843, 0.02745));
	grassColors.push_back(vec3(0.80784, 0.83921, 0.18823));
	grassColors.push_back(vec3(0.90588, 0.90980, 0.05490));
	grassColors.push_back(vec3(0.65882, 0.74117, 0.02352));
	grassColors.push_back(vec3(0.58431, 0.54509, 0.13725));
	grassColors.push_back(vec3(0.31372, 0.46666, 0.01960));
	grassColors.push_back(vec3(0.31372, 0.46666, 0.01960));
}

vec3 Terrain::CenterPosition()
{
	int centerI = totalPatches / 2;
	int centerJ = totalPatches / 2;

	return vertices[centerI * (totalPatches + 1) + centerJ];
}

vector<vec3> Terrain::GetNearbyVertices(vec3 position)
{
	int patchI = position.z / patchHeight;
	int patchJ = position.x / patchWidth;
	int startingPoint = (patchI) * (totalPatches + 1) + patchJ;

	vector<vec3> nearbyVertices;

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			int index = startingPoint + i * (totalPatches + 1) + j;
			if (index < 0 || index >= vertices.size())
			{
				return vector<vec3>();
			}
			nearbyVertices.push_back(vertices[index]);
		}
	}

	return nearbyVertices;
}

void Terrain::SetRenderedPatches()
{
	vec3 cameraPosition = camera->CameraPosition();
	int neighbourPatches = 11;

	int patchI = cameraPosition.z / patchHeight;
	int patchJ = cameraPosition.x / patchWidth;

	indices.clear();

	for (int i = -neighbourPatches; i < neighbourPatches + 1; i++)
	{
		for (int j = -neighbourPatches; j < neighbourPatches + 1; j++)
		{
			if (i + patchI < 0 || i + patchI >= totalPatches)
			{
				continue;
			}
			if (j + patchJ < 0 || j + patchJ >= totalPatches)
			{
				continue;
			}

			int startingPoint = (i + patchI) * (totalPatches + 1) + (j + patchJ);

			indices.push_back(startingPoint);
			indices.push_back(startingPoint + 1);
			indices.push_back(startingPoint + totalPatches + 2);
			indices.push_back(startingPoint + totalPatches + 1);
		}
	}

	glBindVertexArray(vaoID);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), (GLvoid*)&indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void Terrain::SetGoalPosition()
{
	srand(time(NULL));
	int goalI = rand() % totalPatches;
	int goalJ = rand() % totalPatches;
	
	int startigPoint = goalI * (totalPatches + 1) + goalJ;
	verticesColors[startigPoint] = goalColor;
	verticesColors[startigPoint + 1] = goalColor;
	verticesColors[startigPoint + totalPatches + 2] = goalColor;
	verticesColors[startigPoint + totalPatches + 1] = goalColor;

	goalPosition = vertices[startigPoint] + vertices[startigPoint + totalPatches + 2];
	goalPosition /= 2;
}

vec3 Terrain::GoalPosition()
{
	return goalPosition;
}