#include "Sphere.h"

Sphere::Sphere(glm::vec3 _position, float _radius)
{
	mPosition = _position;
	mRadius = _radius;
	mAlbedo = glm::vec3(1, 0, 0);
}

Sphere::Sphere(glm::vec3 _position, float _radius, glm::vec3 _albedo)
{
	mPosition = _position;
	mRadius = _radius;
	mAlbedo = _albedo;
}

bool Sphere::RayIntersect(Ray _ray, glm::vec3& _intersectPosition)
{
	glm::vec3 L = mPosition - _ray.origin;
	float tc = glm::dot(L, _ray.direction);

	if (tc < 0.0f) return false;
	float d2 = glm::dot(L, L) - (tc * tc);

	float radius2 = mRadius * mRadius;
	if (d2 > radius2) return false;

	float t1c = glm::sqrt(radius2 - d2);

	float t1 = tc - t1c;

	_intersectPosition = _ray.origin + (_ray.direction * t1);

	return true;
}

glm::vec3 Sphere::ShadeAtPosition(glm::vec3 _intersectPosition)
{
	return mAlbedo;
}

glm::vec3 Sphere::NormalAtPosition(glm::vec3 _intersectPosition)
{
	return glm::normalize(_intersectPosition - mPosition);
}