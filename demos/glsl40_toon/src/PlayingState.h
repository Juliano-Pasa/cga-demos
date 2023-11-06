#pragma once

#include "GL/glew.h"
#include <GLFW/glfw3.h>

#include "GameState.h"
#include "Camera.h"
#include "Sphere.h"
#include "Scene.h"

#include <vector>

using namespace std;

class PlayingState : public GameState
{
private:
	Camera* camera;
	vector<Scene*> scenes;
	bool loaded;
	bool wireframe;

	GLFWwindow* window;

	void InitializeGL();

public:
	PlayingState(GLFWwindow* window);

	void OnStart();
	void OnPlay();
	void OnPause();
	void OnResume();
	void OnExit();
};
