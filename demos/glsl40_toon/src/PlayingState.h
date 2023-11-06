#pragma once

#include "GameState.h"
#include "Camera.h"
#include "Scene.h"
#include <vector>

using namespace std;

class PlayingState : public GameState
{
private:
	Camera* camera;
	vector<Scene*> scenes;
	bool loaded;

public:
	PlayingState();

	void OnLoad();
	void OnPlay();
	void OnPause();
	void OnResume();
	void OnExit();
};
