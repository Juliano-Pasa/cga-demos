#pragma once

#include "Transform.h"
#include <list>
#include <memory>

using namespace std;

class Entity
{
private:
	list<unique_ptr<Entity>> children;
	Entity* parent;

public:
	Transform transform;

	Entity();

	template<typename... TArgs>
	void AddChild(const TArgs&... args);

	void UpdateSelfAndChildren(bool hasParentChanged);
};