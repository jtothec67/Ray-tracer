#include "Sphere.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
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
    // If the ray origin is inside the sphere, return false
    if (glm::length(_ray.origin - mPosition) < mRadius) return false;

    // Calculate the vector from the ray origin to the sphere center
    glm::vec3 vectorToSphere = mPosition - _ray.origin;
    // Project the vector onto the ray direction
    float projectionLength = glm::dot(vectorToSphere, _ray.direction);

    // If the projection length is negative, the sphere is behind the ray origin, return false
    if (projectionLength < 0.0f) return false;
    // Calculate the squared distance from the sphere center to the ray
    float distanceSquared = glm::dot(vectorToSphere, vectorToSphere) - (projectionLength * projectionLength);

    // Calculate the squared radius of the sphere
    float radiusSquared = mRadius * mRadius;
    // If the distance is greater than the radius, the ray misses the sphere, return false
    if (distanceSquared > radiusSquared) return false;

    // Calculate the half chord length
    float halfChordLength = glm::sqrt(radiusSquared - distanceSquared);

    // Calculate the intersection distance along the ray
    float intersectionDistance = projectionLength - halfChordLength;

    // Calculate the intersection position
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