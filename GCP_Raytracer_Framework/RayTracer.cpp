#include "RayTracer.h"

glm::vec3 RayTracer::TraceRay(Ray _ray)
{
	glm::vec3 lightDir = glm::normalize(glm::vec3(0.f, 0.5f, 0.5f));
	glm::vec3 pixelCol = glm::vec3(0, 0, 0.2f);

	float closestLength = 10000.f;

	for (auto rayObject : rayObjects)
	{
		glm::vec3 hitPos;
		if (rayObject->RayIntersect(_ray, hitPos))
		{
			glm::vec3 hitToOrigin = _ray.origin - hitPos;
			float length = glm::length(hitToOrigin);
			
			if (length < closestLength)
			{
				pixelCol = rayObject->ShadeAtPosition(hitPos, lightDir);
				closestLength = length;
			}
		}
	}

	return pixelCol;
}