#include "Terrain.h"
#include <fstream>
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

	textureManager = TextureManager::Inst();

	glActiveTexture(GL_TEXTURE0);
	if (!textureManager->LoadTexture("..\\..\\resources\\heightMap.png", 0))
		cout << "Failed to load texture." << endl;
}

void Terrain::Update(double deltaTime)
{
	shader.use();
	shader.setUniform("tessLevel", 8);

	shader.setUniform("maxHeight", 100.0f);
	shader.setUniform("heighMapSampler", 0);

	shader.setUniform("color", vec3(0.36f, 0.96f, 0.6f));
	shader.setUniform("lightPos", worldLight->GetPosition());
}

void Terrain::Render(mat4 projection, mat4 view)
{
	shader.use();
	mat4 VPMatrix = projection * view;

	shader.setUniform("VP", VPMatrix);
	shader.setUniform("M", transform.modelMatrix());

	glActiveTexture(GL_TEXTURE0);
	textureManager->BindTexture(0);
	
	glActiveTexture(GL_TEXTURE1);
	textureManager->BindTexture(1);

	glBindVertexArray(vaoID);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawElements(GL_PATCHES, indices.size(), GL_UNSIGNED_INT, (GLubyte*)NULL);
	glBindVertexArray(0);
}

#pragma endregion

#pragma region GenerateFunctions

void Terrain::GenerateVertices()
{
	float maxHeight = 255.0f;
	float size = 1.0f;

	int heightLimit = (height - 1) / totalPatches;
	int widthLimit = (width - 1) / totalPatches;

	vector<vector<float>> heightMap = ReadHeightMap("..\\..\\resources\\heightMap.csv");

	for (int i = 0; i < totalPatches + 1; i++)
	{
		for (int j = 0; j < totalPatches + 1; j++)
		{
			vertices.push_back(vec3(j * widthLimit * totalPatches, heightMap[i * heightLimit][j * widthLimit] * maxHeight, i * heightLimit * totalPatches));
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
}

void Terrain::GenerateBuffers()
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
