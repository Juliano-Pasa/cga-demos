#pragma once

#include "glslprogram.h"
#include "Entity.h"
#include "Terrain.h"
#include <vector>

using namespace std;

class CollisionManager
{
private:
	Entity* referenceEntity;
	vector<Entity*> entities;

	Terrain* terrain;
	vector<vec3> nearbyPatches;

public:
	CollisionManager(vector<Entity*> entities, Entity* referenceEntity, Terrain* terrain);

	void CheckCollisions(bool nearby);
	vec3 InterpolatePosition(vec3 position, vec3 p0, vec3 p1, vec3 p2, vec3 p3);
};