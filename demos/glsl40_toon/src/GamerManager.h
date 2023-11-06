#pragma once

#include "Camera.h"
#include "GameStates.h"

class GameManager
{
private:
	GameState *currentState;

public:
	GameManager();
	void Launch();
};