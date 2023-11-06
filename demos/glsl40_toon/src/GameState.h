#pragma once

class GameState
{
public:
	GameState () {}

	virtual void OnStart() = 0;
	virtual void OnPlay() = 0;
	virtual void OnPause() = 0;
	virtual void OnResume() = 0;
	virtual void OnExit() = 0;
};