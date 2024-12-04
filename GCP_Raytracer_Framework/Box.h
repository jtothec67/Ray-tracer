#pragma once

#include "RayObject.h"

class Box : public RayObject
{
public:
	Box(std::string _name, glm::vec3 _position, glm::vec3 _size);
	Box(std::string _name, glm::vec3 _position, glm::vec3 _size, glm::vec3 _albedo);
	~Box() {}
	bool RayIntersect(Ray _ray, glm::vec3& _intersectPosition) override;
	glm::vec3 NormalAtPosition(glm::vec3 _intersectPosition) override;
	void UpdateUI() override;

	void SetAxis(glm::vec3 _axis) { mAxis = _axis; }
	void SetSize(glm::vec3 _size) { mSize = _size; }

private:
	glm::vec3 mSize{ 1 };
	glm::vec3 mAxis{ 0, 1, 0 };
};