#pragma once

#include "Entity.h"

class WindParticle : public Entity
{
private:


public:

	void Initialize();
	void Update(double deltaTime);
	void Render(mat4 projection, mat4 view);
};