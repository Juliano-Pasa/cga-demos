#pragma once

#include "GL/glew.h"
#include <GLFW/glfw3.h>

#include "GameState.h"
#include "Camera.h"
#include "Entity.h"

#include <vector>

using namespace std;

class PlayingState : public GameState
{
private:
	Camera* camera;
	vector<Entity*> entities;
	bool loaded;
	bool wireframe;

	GLFWwindow* window;

	mat4 projectionMatrix;

	void InitializeGL();

public:
	PlayingState(GLFWwindow* window);

	void OnStart();
	void OnPlay();
	void OnPause();
	void OnResume();
	void OnExit();
};
