#include "PlayerCube.h"
#include <iostream>

PlayerCube::PlayerCube(vec3 position, vec3 rotation, vec3 scale) : Entity(position, rotation, scale)
{
	quality = 100;
	vaoID = 0;

	lightPos = vec3(0.0f, 5.0f, 0.0f);
}


#pragma region EntityFunctions

void PlayerCube::Initialize()
{
	GenSphere();
	GenBuffers();

	try {
		shader.compileShader("shader/glsl40_toon.vert");
		shader.compileShader("shader/glsl40_toon.frag");

		shader.link();
		shader.use();
	}
	catch (GLSLProgramException& e) {
		cerr << e.what() << endl;
		system("pause");
		exit(EXIT_FAILURE);
	}
	shader.printActiveAttribs();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}


void PlayerCube::Update(double deltaTime)
{
	shader.use();
	UpdateSelfAndChildren(false);
}

void PlayerCube::Render(mat4 projection, mat4 view)
{
	shader.use();
	mat4 MVPMatrix = projection * view * transform.modelMatrix();
	shader.setUniform("MVP", MVPMatrix);

	mat3 nm = mat3(glm::inverse(glm::transpose(view * transform.modelMatrix())));
	shader.setUniform("NormalMatrix", nm);
	
	shader.setUniform("LightDir", glm::normalize(worldLight->GetPosition() - transform.position()));
	shader.setUniform("ModelMatrix", transform.modelMatrix());
	shader.setUniform("lightPos", worldLight->GetPosition());

	glBindVertexArray(vaoID);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (GLubyte*)NULL);
	glBindVertexArray(0);
}

#pragma endregion


void PlayerCube::GenBuffers() {

	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);

	unsigned int handle[4];
	glGenBuffers(4, handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), (GLvoid*)&vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(0);  // Vertex position -> layout 0 in the VS

	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), (GLvoid*)&colors[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(1);  // Vertex color -> layout 1 in the VS

	glBindBuffer(GL_ARRAY_BUFFER, handle[2]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), (GLvoid*)&normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)2, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(2);  // Vertex normal -> layout 2 in the VS

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), (GLvoid*)&indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void PlayerCube::GenSphere() {
	vec3 v;
	vec3 n;

	double tetaStep = (2 * glm::pi<float>()) / (double)quality;
	double fiiStep = glm::pi<float>() / (double)quality;

	int tetacount = 0;
	int fiicount = 0;
	int i = 0;
	/// Generates vetrice data
	for (double teta = 0.0; teta <= (2 * glm::pi<float>()); teta += tetaStep)
	{
		tetacount++;
		fiicount = 0;
		for (double fii = 0.0; fii < glm::pi<float>(); fii += fiiStep)
		{
			v[0] = cos(teta) * sin(fii);
			v[1] = sin(teta) * sin(fii);
			v[2] = cos(fii);
			n[0] = v[0];
			n[1] = v[1];
			n[2] = v[2];

			vertices.push_back(v);
			normals.push_back(n);

			fiicount++;
			if (fiicount >= quality)
				break;
		}
		v[0] = cos(teta) * sin(glm::pi<float>());
		v[1] = sin(teta) * sin(glm::pi<float>());
		v[2] = cos(glm::pi<float>());
		n[0] = v[0];
		n[1] = v[1];
		n[2] = v[2];

		vertices.push_back(v);
		normals.push_back(n);

		if (tetacount > quality)
			break;
	}

	/// Make indexes
	int qualMaisUm = quality + 1;
	for (int i = 0; i < quality - 1; i++)
	{
		for (int j = 0; j < qualMaisUm - 1; j++)
		{
			indices.push_back(i * qualMaisUm + j);
			indices.push_back((i + 1) * qualMaisUm + j + 1);
			indices.push_back((i + 1) * qualMaisUm + j);

			indices.push_back(i * qualMaisUm + j);
			indices.push_back(i * qualMaisUm + j + 1);
			indices.push_back((i + 1) * qualMaisUm + j + 1);
		}
	}
	for (int i = 0; i < quality; i++)
	{
		indices.push_back((quality - 1) * qualMaisUm + i);
		indices.push_back(0 + i + 1);
		indices.push_back(0 + i);

		indices.push_back((quality - 1) * qualMaisUm + i);
		indices.push_back((quality - 1) * qualMaisUm + i + 1);
		indices.push_back(0 + i + 1);
	}

	float r = (rand() % 100) / 100.0f;
	float g = (rand() % 100) / 100.0f;
	float b = (rand() % 100) / 100.0f;

	//init colors
	for (int i = 0; i < vertices.size(); i++) {
		colors.push_back(vec3(r, g, b));
	}

	transform.ComputeModelMatrix();
	for (size_t i = 0; i < vertices.size(); i++)
	{
		vec3 x = vertices[i];
		vec3 n = normals[i];
		vec4 model = glm::normalize(vec4(worldLight->GetPosition(), 1) - (transform.modelMatrix() * vec4(x, 1)));

	}
}

