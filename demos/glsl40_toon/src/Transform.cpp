#include "Transform.h"

void Transform::ComputeModelMatrix()
{
	_rotation = quat(_angles);
	_modelMatrix = glm::translate(mat4(1.0f), _position) * glm::toMat4(_rotation) * glm::scale(mat4(1.0f), _scale);
	_hasChanged = false;
}

void Transform::ComputeModelMatrix(const mat4& parentMatrix)
{
	_rotation = quat(_angles);
	_modelMatrix = parentMatrix * glm::translate(mat4(1.0f), _position) * glm::toMat4(_rotation) * glm::scale(mat4(1.0f), _scale);
	_hasChanged = false;
}

vec3 Transform::ClampAngles(const vec3& angles)
{
	vec3 result = angles;
	for (int coord = 0; coord < 3; coord++)
	{
		if (result[coord] < -glm::pi<float>())
		{
			result[coord] += glm::two_pi<float>();
		}
		else if (angles[coord] > glm::pi<float>())
		{
			result[coord] -= glm::two_pi<float>();
		}
	}

	return result;
}