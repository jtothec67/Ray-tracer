#pragma once

#include "RayObject.h"
#include "Light.h"

#include <vector>

class RayTracer
{
public:
	RayTracer() {}
	~RayTracer() {}

	glm::vec3 TraceRay(Ray _ray, glm::vec3 _camPos);

	void SetLights(std::vector<Light>* _lights) { mLights = _lights; }

	std::vector<RayObject*> rayObjects;
	std::vector<Light>* mLights;
};