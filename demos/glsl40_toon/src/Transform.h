#pragma once

#include "glslprogram.h"

struct Transform
{
protected:
	vec3 _position;
	vec3 _scale;
	vec3 _angles;
	quat _rotation;

	mat4 _modelMatrix = mat4(1.0f);

	bool _hasChanged = true;

public:
	const vec3& position() const { return _position; }
	void position(const vec3& position) { _position = position; _hasChanged = true; }

	const vec3& scale() const { return _scale; }
	void scale(const vec3& scale) { _scale = scale; _hasChanged = true; }

	const vec3& angles() const { return _angles; }
	void angles(const vec3& angles) { _angles = angles; _hasChanged = true; }

	const quat& rotation() const { return _rotation; }
	void rotation(const quat& rotation) { _rotation = rotation; _hasChanged = true; }

	const mat4& modelMatrix() const { return _modelMatrix; }
	const bool hasChanged() const { return _hasChanged; }

	void ComputeModelMatrix();
	void ComputeModelMatrix(const mat4& parentMatrix);
};