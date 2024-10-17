#include "Sphere.h"

#include <iostream>

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

bool Sphere::RayIntersect2(Ray _ray, glm::vec3& _intersectPosition)
{
	glm::vec3 L = mPosition - _ray.origin;
	float tca = glm::dot(L, _ray.direction);
	if (tca < 0) return false;
	float d2 = glm::dot(L, L) - tca * tca;
	if (d2 > mRadius * mRadius) return false;
	float thc = sqrt(mRadius * mRadius - d2);
	float t0 = tca - thc;
	float t1 = tca + thc;

	if (t0 > t1) std::swap(t0, t1);

	if (t0 < 0)
	{
		t0 = t1;
		if (t0 < 0) return false;
	}

	_intersectPosition = _ray.origin + _ray.direction * t0;
	return true;
}

bool Sphere::RayIntersect3(Ray _ray, glm::vec3& _intersectPosition)
{
	glm::vec3 oc = _ray.origin - mPosition;
	float a = glm::dot(_ray.direction, _ray.direction);
	float b = 2.0f * glm::dot(oc, _ray.direction);
	float c = glm::dot(oc, oc) - mRadius * mRadius;

	// Solve the quadratic equation for t
	float discriminant = b * b - 4.0f * a * c;

	// Check if the discriminant is negative, meaning no intersection
	if (discriminant < 0) {
		return false;
	}

	// Calculate the nearest t value (smallest positive solution)
	float sqrtDiscriminant = std::sqrt(discriminant);
	float t1 = (-b - sqrtDiscriminant) / (2.0f * a);
	float t2 = (-b + sqrtDiscriminant) / (2.0f * a);

	// Check for the nearest positive t value (t1 or t2)
	float t = (t1 > 0) ? t1 : t2;

	// If t is negative, the intersection is behind the ray origin
	if (t < 0) {
		return false;
	}

	// Calculate the intersection point using the ray equation
	_intersectPosition = _ray.origin + t * _ray.direction;
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