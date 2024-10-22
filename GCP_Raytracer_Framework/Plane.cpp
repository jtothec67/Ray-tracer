#include "Plane.h"

Plane::Plane(glm::vec3 _position, glm::vec3 _normal, glm::vec3 _albedo)
{
	mPosition = _position;
	mNormal = _normal;
	mAlbedo = _albedo;
}

bool Plane::RayIntersect(Ray _ray, glm::vec3& _intersectPosition)
{
	float denom = glm::dot(_ray.direction, mNormal);

	float t = glm::dot((mPosition - _ray.origin), mNormal) / denom;
	
	if (t < 0) return false;

	_intersectPosition = _ray.origin + (t * _ray.direction);

	return true;
}

glm::vec3 Plane::ShadeAtPosition(glm::vec3 _intersectPosition)
{
	return mAlbedo;
}

glm::vec3 Plane::NormalAtPosition(glm::vec3 _intersectPosition)
{
	return mNormal;
}