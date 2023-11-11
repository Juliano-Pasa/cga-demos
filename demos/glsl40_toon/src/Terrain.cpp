#include "Terrain.h"
#include <iostream>

Terrain::Terrain(int width, int height, int scaleDown, string heightMapPath, string normalMapPath) : Entity(vec3(0), vec3(0), vec3(1))
{
	this->width = width;
	this->height = height;
	this->scaleDown = scaleDown;
	this->vaoID = 0;
}

#pragma region EntityFunctions

void Terrain::Initialize()
{
	GenerateVertices();
	GenerateBuffers();

	textureManager = TextureManager::Inst();

	glActiveTexture(GL_TEXTURE0);
	if (!textureManager->LoadTexture("..\\..\\resources\\heightMap.png", 0))
		cout << "Failed to load texture." << endl;

	glActiveTexture(GL_TEXTURE1);
	if (!textureManager->LoadTexture("..\\..\\resources\\normalMap.csv", 1))
		cout << "Failed to load texture." << endl;
}

void Terrain::Update(double deltaTime)
{

}

void Terrain::Render(mat4 projection, mat4 view)
{
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
	for (int i = 0; i < height; i += scaleDown)
	{
		for (int j = 0; j < width; j += scaleDown)
		{
			vertices.push_back(vec3(j, 0, i));
			texCoords.push_back(vec2(i / (float)(height - 1), j / (float)(width - 1)));
		}
	}

	int indexHeight = height / scaleDown;
	int indexWidth = width / scaleDown;

	for (int i = 0; i < indexHeight - 1; i++)
	{
		for (int j = 0; j < indexWidth - 1; j++)
		{
			int startingPoint = i * indexWidth + j;
			indices.push_back(startingPoint);
			indices.push_back(startingPoint + 1);
			indices.push_back(startingPoint + indexWidth + 1);
			indices.push_back(startingPoint + indexWidth);
		}
	}
}

void Terrain::GenerateBuffers()
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
	glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(vec2), (GLvoid*)&texCoords[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), (GLvoid*)&indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

#pragma endregion
