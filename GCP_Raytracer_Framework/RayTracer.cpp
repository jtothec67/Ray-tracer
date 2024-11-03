#include "RayTracer.h"

glm::vec3 RayTracer::TraceRay(Ray _ray, glm::vec3 _camPos)
{
	glm::vec3 currentHitPos;
	RayObject* currentRayObject = nullptr;

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
				currentRayObject = rayObject;
				currentHitPos = hitPos;
				closestLength = length;
			}
		}
	}

	if (currentRayObject == nullptr)
		return glm::vec3(0, 0, 0.2f);

	glm::vec3 finalPixelCol = glm::vec3(0.f, 0.f, 0.f);

	for (auto &light : *mLights)
	{
		glm::vec3 thisPixelCol = currentRayObject->ShadeAtPosition(currentHitPos, glm::normalize(light.position - currentHitPos), light.colour, _camPos);

		for (auto rayObject : rayObjects)
		{
			if (rayObject->mIsLight)
				continue;

			glm::vec3 hitPos;

			glm::vec3 shadowRayFrom = currentHitPos + (currentRayObject->NormalAtPosition(currentHitPos) * 0.01f);

			if (rayObject->RayIntersect(Ray(shadowRayFrom, glm::normalize(light.position - currentHitPos)), hitPos))
			{
				if (glm::length(hitPos - currentHitPos) > glm::length(light.position - currentHitPos))
					continue;

				thisPixelCol = glm::vec3(0, 0, 0);
			}
		}

		finalPixelCol += thisPixelCol;
	}

	return finalPixelCol;
}