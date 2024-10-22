#pragma once

#include "Ray.h"

#include <GLM/glm.hpp>

class RayObject
{
public:
	RayObject() {}
	~RayObject() {}

	glm::vec3 GetAlbedo() { return mAlbedo; }

	virtual bool RayIntersect(Ray _ray, glm::vec3& _intersectPosition) = 0;
	virtual glm::vec3 ShadeAtPosition(glm::vec3 _intersectPosition, glm::vec3 _lightDir) = 0;
	virtual glm::vec3 NormalAtPosition(glm::vec3 _intersectPosition) = 0;

protected:
	glm::vec3 mPosition{ 0 };
	glm::vec3 mAlbedo{ 0 };
};