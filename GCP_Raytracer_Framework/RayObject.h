#pragma once

#include "Ray.h"
#include "Light.h"

#include <GLM/glm.hpp>

#include <vector>

class RayObject
{
public:
	RayObject() {}
	~RayObject() {}

	virtual bool RayIntersect(Ray _ray, glm::vec3& _intersectPosition) = 0;
	virtual glm::vec3 NormalAtPosition(glm::vec3 _intersectPosition) = 0;

	glm::vec3 ShadeAtPosition(glm::vec3 _intersectPosition, glm::vec3 lightDir, glm::vec3 _lightCol, glm::vec3 _camPos);

	glm::vec3 mPosition{ 0 };
	glm::vec3 mAlbedo{ 0 };
	float mShininess = 1;

	bool mIsLight = false;
};