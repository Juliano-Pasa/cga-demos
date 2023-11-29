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
	camera = new Camera(window, vec3(-30, 200, -30));
	projectionMatrix = glm::perspective(glm::radians(60.0f), (float)windowWidth/(float)windowHeight, 0.1f, 10000.0f);
	worldLight = new WorldLight(vec3(1, 1, 1), vec3(300, 2000, 300));

	InitializeGL();
	InitializeTerrain();

	srand((unsigned)time(NULL));

	PlayerCube* cube = new PlayerCube(vec3(-30, 200, -30), vec3(0, 0, 0), vec3(1.1));
	cube->worldLight = worldLight;
	entities.push_back(cube);
	entities.back()->Initialize();
	terrain->AddChild(entities.back());

	PlayerCube* cube2 = new PlayerCube(vec3(133, 200, 127), vec3(0, 0, 0), vec3(1));
	cube2->worldLight = worldLight;
	entities.push_back(cube2);
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
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

void PlayingState::InitializeTerrain()
{
	int mapSize = 8;
	TerrainGenerator terrainGenerator = TerrainGenerator(mapSize, vector<double>{64, 128, 192, 128}, 512, 0.2f, 42);
	string heightMapPath = "..\\..\\resources\\heightMap.csv";
	string normalMapPath = "..\\..\\resources\\normalMap.csv";

	terrainGenerator.GenerateDiamondSquare();
	terrainGenerator.WriteHeightMapToCSV(heightMapPath);
	terrainGenerator.FreeMap();

	int dimensions = (int)pow(2, mapSize) + 1;
	terrain = new Terrain(dimensions, dimensions, 256, camera, heightMapPath, normalMapPath);
	terrain->worldLight = worldLight;
	terrain->Initialize();
	entities.push_back(terrain);
}

#pragma endregion
