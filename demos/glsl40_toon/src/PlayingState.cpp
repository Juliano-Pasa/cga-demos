#include "PlayingState.h"
#include "PlayerCube.h"
#include "Duck.h"
#include "TerrainGenerator.h"
#include "DuckPlayerControler.h"
#include "DuckBotControler.h"
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
	hiddenCursor = true;
	entities = vector<Entity*>();

	this->window = window;
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
}

#pragma region StateTransitions

void PlayingState::OnStart()
{
	InitializeInputManager(window);

	camera = new Camera(inputManager, vec3(3000, 600, 3000));
	projectionMatrix = glm::perspective(glm::radians(60.0f), (float)windowWidth / (float)windowHeight, 0.1f, 10000.0f);
	worldLight = new WorldLight(vec3(1, 1, 1), vec3(1000, 20000, 1000));

	InitializeGL();
	InitializeTerrain();

	vec3 terrainCenterPosition = terrain->CenterPosition();

	PlayerCube* cube2 = new PlayerCube(vec3(3050, 600, 3000), vec3(0, 0, 0), vec3(30));
	cube2->worldLight = worldLight;
	entities.push_back(cube2);
	entities.back()->Initialize();
	terrain->AddChild(entities.back());

	Entity* duck = InitializePlayer(inputManager, camera, worldLight);
	camera->SetEntityReference(duck);

	InitializeBots(worldLight);

	vector<Entity*> collidables = vector<Entity*>{ duck, entities.back()};
	collisionManager = new CollisionManager(collidables, duck, terrain);

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
		ReadKeyboardInput();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		deltaTime = glfwGetTime() - lastTime;

		for (Entity* entity : entities)
		{
			entity->Update(deltaTime);
		}

		collisionManager->CheckCollisions(false);
		camera->Update(deltaTime);

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
	glClearColor(0.47055f, 0.55289f, 0.86785f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

void PlayingState::InitializeTerrain()
{
	int mapSize = 8;
	TerrainGenerator terrainGenerator = TerrainGenerator(mapSize, vector<double>{64, 128, 192, 128}, 512, 0.2f, 42);
	string heightMapPath = "..\\..\\resources\\heightMap.csv";
	string normalMapPath = "..\\..\\resources\\normalMap.csv";

	//terrainGenerator.GenerateDiamondSquare();
	//terrainGenerator.WriteHeightMapToCSV(heightMapPath);
	//terrainGenerator.FreeMap();

	int dimensions = (int)pow(2, mapSize) + 1;
	terrain = new Terrain(dimensions, dimensions, 256, camera, heightMapPath, normalMapPath);
	terrain->worldLight = worldLight;
	terrain->Initialize();
	entities.push_back(terrain);
}

void PlayingState::InitializeInputManager(GLFWwindow* window)
{
	vector<int> keys = { GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_LEFT_SHIFT, GLFW_KEY_LEFT_CONTROL, GLFW_KEY_ESCAPE };
	inputManager = new InputManager(keys, true);
	inputManager->SetupInputs(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

Entity* PlayingState::InitializePlayer(InputManager* inputManager, Camera* camera, WorldLight* worldLight)
{
	DuckPlayerControler* controler = new DuckPlayerControler();
	controler->Initialize(inputManager, camera, 1000.0f, 500.0f, 250.0f, 350.0f, 1.0f);

	Duck* duck = new Duck(camera->CameraPosition(), vec3(70), worldLight, controler);
	entities.push_back(duck);
	entities.back()->Initialize();
	terrain->AddChild(entities.back());

	return duck;
}

void PlayingState::InitializeBots(WorldLight* worldLight)
{
	DuckBotControler* controler = new DuckBotControler();
	controler->Initialize(1000.0f, 500.0f, 250.0f, 350.0f, 1.0f);

	Duck* duck = new Duck(camera->CameraPosition(), vec3(70), worldLight, controler);
	entities.push_back(duck);
	entities.back()->Initialize();
	terrain->AddChild(entities.back());
}

#pragma endregion

void PlayingState::ReadKeyboardInput()
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

	if (glfwGetKeyOnce(window, 'Z')) {
		hiddenCursor = !hiddenCursor;
		if (hiddenCursor) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
}
