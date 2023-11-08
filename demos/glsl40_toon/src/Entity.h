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

	Entity(vec3 position, vec3 rotation, vec3 scale);

	template<typename... TArgs>
	void AddChild(const TArgs&... args);

	void UpdateSelfAndChildren(bool hasParentChanged);

	virtual void Initialize() = 0;
	virtual void Update(double deltaTime) = 0;
	virtual void Render(mat4 projection, mat4 view) = 0;
};