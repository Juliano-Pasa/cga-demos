#pragma once

#include "Entity.h"
#include <vector>

using namespace std;

class EntityControler
{
protected:
	Entity* entity;
	bool initialized;

public:
	EntityControler();
	void SetEntity(Entity* entity);

	virtual void Update(float deltaTime) = 0;
};