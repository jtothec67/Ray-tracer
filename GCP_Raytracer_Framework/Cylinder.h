#pragma once

#include "RayObject.h"

class Cylinder : public RayObject
{
public:
	Cylinder(std::string _name, glm::vec3 _position, float _radius, float _height);
	Cylinder(std::string _name, glm::vec3 _position, float _radius, float _height, glm::vec3 _albedo);
	~Cylinder() {}

	bool RayIntersect(Ray _ray, glm::vec3& _intersectPosition) override;
	glm::vec3 NormalAtPosition(glm::vec3 _intersectPosition) override;

	void SetAxis(glm::vec3 _axis) { mAxis = _axis; }
	void SetRadius(float _radius) { mRadius = _radius; }
	void SetHeight(float _height) { mHeight = _height; }

	void UpdateUI() override;

private:
	float mRadius = 0;
	float mHeight = 0;
	glm::vec3 mAxis{ 0, 1, 0 };
};