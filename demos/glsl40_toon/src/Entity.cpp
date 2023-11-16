#include "Entity.h"

Entity::Entity(vec3 position, vec3 rotation, vec3 scale)
{
	transform.position(position);
	transform.angles(rotation);
	transform.scale(scale);
	parent = nullptr;
	transform.ComputeModelMatrix();
}

void Entity::AddChild(Entity* entity)
{
	children.push_back(entity);
	children.back()->parent = this;
}

void Entity::UpdateSelfAndChildren(bool hasParentChanged)
{
	if (transform.hasChanged() || hasParentChanged)
	{
		if (parent)
		{
			transform.ComputeModelMatrix(parent->transform.modelMatrix());
		}
		else
		{
			transform.ComputeModelMatrix();
		}

		for (Entity* child : children)
		{
			child->UpdateSelfAndChildren(true);
		}
		return;
	}

	for (Entity* child : children)
	{
		child->UpdateSelfAndChildren(false);
	}
}