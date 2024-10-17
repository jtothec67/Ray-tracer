#pragma once

#include "Sphere.h"

#include <vector>

class RayTracer
{
public:
	RayTracer() {}
	~RayTracer() {}

	glm::vec3 TraceRay(Ray _ray);

	std::vector<Sphere*> mSpheres;
};