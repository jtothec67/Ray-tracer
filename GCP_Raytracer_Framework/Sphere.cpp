#include "Sphere.h"

#include <iostream>

Sphere::Sphere(glm::vec3 _position, float _radius)
{
	mPosition = _position;
	mRadius = _radius;
	mAlbedo = glm::vec3(1, 0, 0);
}

Sphere::Sphere(glm::vec3 _position, float _radius, glm::vec3 _albedo, float _shininess)
{
	mPosition = _position;
	mRadius = _radius;
	mAlbedo = _albedo;
	mShininess = _shininess;
}

bool Sphere::RayIntersect(Ray _ray, glm::vec3& _intersectPosition)
{
	if (glm::length(_ray.origin - mPosition) < mRadius) return false;

	glm::vec3 vectorToSphere = mPosition - _ray.origin;
	float projectionLength = glm::dot(vectorToSphere, _ray.direction);

	if (projectionLength < 0.0f) return false;
	float distanceSquared = glm::dot(vectorToSphere, vectorToSphere) - (projectionLength * projectionLength);

	float radiusSquared = mRadius * mRadius;
	if (distanceSquared > radiusSquared) return false;

	float halfChordLength = glm::sqrt(radiusSquared - distanceSquared);

	float intersectionDistance = projectionLength - halfChordLength;

	_intersectPosition = _ray.origin + (_ray.direction * intersectionDistance);
	return true;
}

glm::vec3 Sphere::NormalAtPosition(glm::vec3 _intersectPosition)
{
	return glm::normalize(_intersectPosition - mPosition);
}