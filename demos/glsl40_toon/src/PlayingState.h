#pragma once

#include "GL/glew.h"
#include <GLFW/glfw3.h>

#include "GameState.h"
#include "Camera.h"
#include "Terrain.h"
#include "Entity.h"
#include "WorldLight.h"

#include <vector>

using namespace std;

class PlayingState : public GameState
{
private:
	Camera* camera;
	Terrain* terrain;
	WorldLight* worldLight;

	vector<Entity*> entities;
	bool loaded;
	bool wireframe;

	GLFWwindow* window;
	int windowWidth;
	int windowHeight;

	mat4 projectionMatrix;

	void InitializeGL();
	void InitializeTerrain();

public:
	PlayingState(GLFWwindow* window);

	void OnStart();
	void OnPlay();
	void OnPause();
	void OnResume();
	void OnExit();
};
