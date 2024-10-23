#pragma once

#include "Ray.h"
#include "RayObject.h"

#include <GLM/glm.hpp>

class Sphere : RayObject
{
public:
	Sphere(glm::vec3 _position, float _radius);
	Sphere(glm::vec3 _position, float _radius, glm::vec3 _albedo);
	~Sphere() {}

	bool RayIntersect(Ray _ray, glm::vec3& _intersectPosition) override;
	glm::vec3 NormalAtPosition(glm::vec3 _intersectPosition) override;

private:
	float mRadius = 0;
};