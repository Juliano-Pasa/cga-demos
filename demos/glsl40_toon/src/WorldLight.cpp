#include "WorldLight.h"
#include <iostream>

WorldLight::WorldLight(vec3 color, vec3 position) : Entity(position, vec3(0), vec3(1))
{
	this->color = color;
}

const vec3& WorldLight::GetColor()
{
	return color;
}

const vec3& WorldLight::GetPosition()
{
	return transform.position();
}

#pragma region EntityFunctions

void WorldLight::Initialize()
{
}
void WorldLight::Update(double deltaTime)
{
}
void WorldLight::Render(mat4 projection, mat4 view)
{
}
#pragma endregion
