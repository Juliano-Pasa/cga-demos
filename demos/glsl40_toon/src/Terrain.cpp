#include "Terrain.h"
#include <iostream>

Terrain::Terrain(int width, int height, int scaleDown, Camera* camera, string heightMapPath, string normalMapPath) : Entity(vec3(0), vec3(0), vec3(1))
{
	this->width = width;
	this->height = height;
	this->scaleDown = scaleDown;
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
	shader.setUniform("minTess", 16);
	shader.setUniform("maxTess", 128);
	shader.setUniform("maxDist", 250.0f);
	shader.setUniform("camPos", camera->CameraPosition());

	shader.setUniform("maxHeight", 100.0f);
	shader.setUniform("heighMapSampler", 0);

	shader.setUniform("color", vec3(0.36f, 0.96f, 0.6f));
	shader.setUniform("lightPos", worldLight->GetPosition());
}

void Terrain::Render(mat4 projection, mat4 view)
{
	shader.use();
	mat4 VPMatrix = projection * view;
	mat3 nm = mat3(glm::inverse(glm::transpose(view * transform.modelMatrix())));

	shader.setUniform("VP", VPMatrix);
	shader.setUniform("M", transform.modelMatrix());
	shader.setUniform("NM", nm);

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
	int heightLimit = height / scaleDown;
	int widthLimit = width / scaleDown;
	float size = 50.0f;

	for (int i = 0; i < scaleDown + 1; i++)
	{
		for (int j = 0; j < scaleDown + 1; j++)
		{
			vertices.push_back(vec3(j * widthLimit, 0, i * heightLimit));
			texCoords.push_back(vec2(i / float(scaleDown), j / float(scaleDown)));
		}
	}

	for (int i = 0; i < scaleDown; i++)
	{
		for (int j = 0; j < scaleDown; j++)
		{
			int startingPoint = i * (scaleDown + 1) + j;
			indices.push_back(startingPoint);
			indices.push_back(startingPoint + 1);
			indices.push_back(startingPoint + scaleDown + 2);
			indices.push_back(startingPoint + scaleDown + 1);
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
