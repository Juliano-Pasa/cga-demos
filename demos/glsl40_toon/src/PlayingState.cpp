#include "PlayingState.h"
#include "PlayerCube.h"
#include "TerrainGenerator.h"
#include <ctime>

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
	TerrainGenerator terrain = TerrainGenerator(8, vector<unsigned char>{128, 128, 128, 128}, 256, 0.4f, 42);
	terrain.GenerateDiamondSquare();
	terrain.WriteHeightMapToPNG("..\\..\\resources\\heightMap.png");
	terrain.GenerateNormalMap();
	terrain.WriteNormalMapToPNG("..\\..\\resources\\normalMap.png");

	InitializeGL();
	srand((unsigned)time(NULL));

	camera = new Camera(window, vec3(0, 0, 0));
	projectionMatrix = glm::perspective(glm::radians(60.0f), (float)windowWidth/(float)windowHeight, 0.1f, 200.0f);

	entities.push_back(new PlayerCube(vec3(5, 0, 1), vec3(0, 0, 0), vec3(1, 1, 1)));
	entities.back()->Initialize();

	entities.push_back(new PlayerCube(vec3(5, 0, -1), vec3(0, 0, 0), vec3(1, 1, 1)));
	entities.back()->Initialize();

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

#pragma endregion
