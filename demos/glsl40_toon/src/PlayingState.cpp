#include "PlayingState.h"

PlayingState::PlayingState() : GameState()
{
	loaded = false;
	camera = nullptr;
	scenes = vector<Scene*>();
}

#pragma region StateTransitions

void PlayingState::OnLoad()
{
	


	loaded = true;
}

void PlayingState::OnPlay()
{
	if (!loaded)
	{
		return;
	}
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
