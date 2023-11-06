#include "glslprogram.h"

struct Transform
{
	vec3 position;
	vec3 scale;
	vec3 angles;
	quat rotation;

	mat4 modelMatrix = mat4(1.0f);
};