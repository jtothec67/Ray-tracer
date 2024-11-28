#include "Sphere.h"

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>

Sphere::Sphere(std::string _name, glm::vec3 _position, float _radius)
{
	mName = _name;
	mPosition = _position;
	mRadius = _radius;
	mAlbedo = glm::vec3(1, 0, 0);
}

Sphere::Sphere(std::string _name, glm::vec3 _position, float _radius, glm::vec3 _albedo)
{
	mName = _name;
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

glm::vec3 Sphere::NormalAtPosition(glm::vec3 _intersectPosition)
{
	return glm::normalize(_intersectPosition - mPosition);
}

void Sphere::UpdateUI()
{
	if (mIsLight) 
		return;

	RayObject::UpdateUI();

	ImGui::SliderFloat(("Radius" + mName).c_str(), &mRadius, 0.0f, 20.0f);
}