#include "Plane.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>

Plane::Plane(std::string _name, glm::vec3 _position, glm::vec3 _normal, glm::vec3 _albedo)
{
	mName = _name;
	mPosition = _position;
	mNormal = _normal;
	mAlbedo = _albedo;
}

bool Plane::RayIntersect(Ray _ray, glm::vec3& _intersectPosition)
{
	// Calculate denominator
	float denom = glm::dot(_ray.direction, mNormal);

	float t = glm::dot((mPosition - _ray.origin), mNormal) / denom;
	
	if (t < 0) return false;

	_intersectPosition = _ray.origin + (t * _ray.direction);

	return true;
}

glm::vec3 Plane::NormalAtPosition(glm::vec3 _intersectPosition)
{
	// Store normal so can just return it
	return glm::normalize(mNormal);
}

void Plane::UpdateUI()
{
	if (mIsLight)
		return;

	RayObject::UpdateUI();

	ImGui::SliderFloat3(("Normal " + mName).c_str(), &mNormal[0], -1, 1);
}