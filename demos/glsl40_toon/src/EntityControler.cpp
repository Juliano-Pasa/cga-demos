#include "EntityControler.h"

EntityControler::EntityControler()
{
	entity = nullptr;
	initialized = false;
}

void EntityControler::SetEntity(Entity* entity)
{
	this->entity = entity;
}
