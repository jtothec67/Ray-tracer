#pragma once

#include "Ray.h"

#include <GLM/glm.hpp>

class Sphere
{
public:
	Sphere(glm::vec3 _position, float _radius);
	Sphere(glm::vec3 _position, float _radius, glm::vec3 _albedo);
	~Sphere() {}

	glm::vec3 GetAlbedo() { return mAlbedo; }

	bool RayIntersect(Ray _ray, glm::vec3& _intersectPosition);
	bool RayIntersect2(Ray _ray, glm::vec3& _intersectPosition);
	bool RayIntersect3(Ray _ray, glm::vec3& _intersectPosition);
	glm::vec3 ShadeAtPosition(glm::vec3 _intersectPosition);
	glm::vec3 NormalAtPosition(glm::vec3 _intersectPosition);

private:
	glm::vec3 mPosition;
	glm::vec3 mAlbedo;
	float mRadius = 0;
};