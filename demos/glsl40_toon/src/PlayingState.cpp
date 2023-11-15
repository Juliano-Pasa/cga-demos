#include "PlayingState.h"
#include "PlayerCube.h"
#include "TerrainGenerator.h"
#include <ctime>
#include <iostream>

char keyOnce[GLFW_KEY_LAST + 1];
#define glfwGetKeyOnce(WINDOW, KEY)             \
    (glfwGetKey(WINDOW, KEY) ?              \
     (keyOnce[KEY] ? false : (keyOnce[KEY] = true)) :   \
     (keyOnce[KEY] = false))

PlayingState::PlayingState(GLFWwindow* window) : GameState()
{
	loaded = false;
	camera = nullptr;
	wireframe = false;
	entities = vector<Entity*>();

	this->window = window;
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
}

#pragma region StateTransitions

void PlayingState::OnStart()
{
	camera = new Camera(window, vec3(128, 200, 128));
	projectionMatrix = glm::perspective(glm::radians(60.0f), (float)windowWidth/(float)windowHeight, 0.1f, 500.0f);
	worldLight = new WorldLight(vec3(1, 1, 1), vec3(133, 200, 129));

	InitializeGL();
	InitializeTerrain();

	srand((unsigned)time(NULL));

	entities.push_back(new PlayerCube(vec3(133, 200, 129), vec3(0, 0, 0), vec3(1)));
	entities.back()->Initialize();
	terrain->AddChild(entities.back());

	entities.push_back(new PlayerCube(vec3(133, 200, 127), vec3(0, 0, 0), vec3(1)));
	entities.back()->Initialize();
	terrain->AddChild(entities.back());

	loaded = true;
	OnPlay();
}

void PlayingState::OnPlay()
{
	if (!loaded)
	{
		return;
	}

	double deltaTime;
	double lastTime = glfwGetTime();
	do
	{
		if (glfwGetKeyOnce(window, 'Q')) {
			wireframe = !wireframe;
			if (wireframe) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
			else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		deltaTime = glfwGetTime() - lastTime;
		camera->Update(deltaTime);

		for (Entity* entity : entities)
		{
			entity->Update(deltaTime);
		}
		lastTime += deltaTime;

		for (Entity* entity : entities)
		{
			entity->Render(projectionMatrix, camera->viewMatrix);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (!glfwWindowShouldClose(window));
}

void PlayingState::OnPause()
{
	if (!loaded)
	{
		return;
	}
}

void PlayingState::OnResume()
{
	if (!loaded)
	{
		return;
	}
}

void PlayingState::OnExit()
{
	if (!loaded)
	{
		return;
	}
}

#pragma endregion


#pragma region InitializingFunctions

void PlayingState::InitializeGL()
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
}

void PlayingState::InitializeTerrain()
{
	int mapSize = 8;
	TerrainGenerator terrainGenerator = TerrainGenerator(mapSize, vector<unsigned char>{128, 128, 128, 128}, 512, 0.9f, 42);
	string heightMapPath = "..\\..\\resources\\heightMap.png";
	string normalMapPath = "..\\..\\resources\\normalMap.png";

	terrainGenerator.GenerateDiamondSquare();
	terrainGenerator.WriteHeightMapToPNG(heightMapPath);
	terrainGenerator.GenerateNormalMap();
	terrainGenerator.WriteNormalMapToPNG(normalMapPath);

	int dimensions = (int)pow(2, mapSize) + 1;
	terrain = new Terrain(dimensions, dimensions, 2, camera, heightMapPath, normalMapPath);
	terrain->worldLight = worldLight;
	terrain->Initialize();
	entities.push_back(terrain);
}

#pragma endregion
