#pragma once

#include "RayObject.h"

class Plane : RayObject
{
public:
	Plane(glm::vec3 _position, glm::vec3 _normal, glm::vec3 _albedo);
	~Plane() {}

	bool RayIntersect(Ray _ray, glm::vec3& _intersectPosition) override;
	glm::vec3 ShadeAtPosition(glm::vec3 _intersectPosition, glm::vec3 _lightDir) override;
	glm::vec3 NormalAtPosition(glm::vec3 _intersectPosition) override;

private:
	glm::vec3 mNormal{ 0 };
};