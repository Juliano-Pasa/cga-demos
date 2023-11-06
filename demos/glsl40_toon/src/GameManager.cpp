#include "GamerManager.h"

GameManager::GameManager()
{
	currentState = nullptr;
}

void GameManager::Launch()
{
	currentState = new PlayingState();
}