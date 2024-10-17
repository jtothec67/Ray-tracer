#include "RayTracer.h"

glm::vec3 RayTracer::TraceRay(Ray _ray)
{
	for (auto sphere : mSpheres)
	{
		glm::vec3 hitPos;
		if (sphere->RayIntersect(_ray, hitPos))
		{
			return sphere->ShadeAtPosition(hitPos);
			break;
		}
	}

	return glm::vec3(0, 0, 0);
}