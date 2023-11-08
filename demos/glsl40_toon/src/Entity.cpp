#include "Entity.h"

Entity::Entity(vec3 position, vec3 rotation, vec3 scale)
{
	transform.position(position);
	transform.angles(rotation);
	transform.scale(scale);
	parent = nullptr;
}

template<typename... TArgs>
void Entity::AddChild(const TArgs&... args)
{
	children.emplace_back(std::make_unique<Entity>(args...));
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

		for (unique_ptr<Entity>& child : children)
		{
			child->UpdateSelfAndChildren(true);
		}
		return;
	}

	for (unique_ptr<Entity>& child : children)
	{
		child->UpdateSelfAndChildren(false);
	}
}