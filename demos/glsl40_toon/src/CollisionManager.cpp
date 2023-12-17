#include "CollisionManager.h"
#include <iostream>


CollisionManager::CollisionManager(vector<Entity*> entities, Entity* referenceEntity, Terrain* terrain)
{
	this->entities = entities;
	this->referenceEntity = referenceEntity;
	this->terrain = terrain;
}

void CollisionManager::CheckCollisions(bool nearby)
{
	for (Entity* entity : entities)
	{
		vector<vec3> nearbyVertices = terrain->GetNearbyVertices(entity->transform.position());

		if (nearbyVertices.size() == 0)
		{
			entity->transform.position(terrain->CenterPosition() + 10.0f);
			continue;
		}

		vec3 entityPosition = entity->transform.position();
		vec3 interpolatedPosition = InterpolatePosition(
			entityPosition,
			nearbyVertices[0],
			nearbyVertices[1],
			nearbyVertices[3],
			nearbyVertices[2]);

		entityPosition.y = interpolatedPosition.y + 10.0f;
		entity->transform.position(entityPosition);
	}
}

vec3 CollisionManager::InterpolatePosition(vec3 position, vec3 p0, vec3 p1, vec3 p2, vec3 p3)
{
	float u = (position.x - p0.x) / (p2.x - p0.x);
	float v = (position.z - p0.z) / (p2.z - p0.z);

	return p0 * (1 - u) * (1 - v) +
		p1 * u * (1 - v) +
		p3 * v * (1 - u) +
		p2 * u * v;
}
