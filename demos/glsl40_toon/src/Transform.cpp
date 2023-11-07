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