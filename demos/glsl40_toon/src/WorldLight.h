#pragma once

#include "Entity.h"

class WorldLight : public Entity
{
private:
	vec3 color;

public:
	WorldLight(vec3 color, vec3 position);

	const vec3& GetPosition();
	const vec3& GetColor();

	void Initialize();
	void Update(double deltaTime);
	void Render(mat4 projection, mat4 view);
};