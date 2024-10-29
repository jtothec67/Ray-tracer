#include "RayObject.h"

#include <iostream>

glm::vec3 RayObject::ShadeAtPosition(glm::vec3 _intersectPosition, glm::vec3 _lightDir)
{
	glm::vec3 finalL = glm::vec3(0, 0, 0);

	for (auto& light : *mLights)
	{
		glm::vec3 lightDir = glm::normalize(light.position - _intersectPosition);
		glm::vec3 lightCol = light.colour;
		glm::vec3 L = (glm::max(glm::dot(lightDir, NormalAtPosition(_intersectPosition)), 0.f)) * mAlbedo * lightCol;

		/*if (L.x < 0 || L.y < 0 || L.z < 0)
			continue;*/

		//L = glm::max(L, glm::vec3(0.0f, 0.0f, 0.0f));

		// Can't decide which is best. Forgetting results with any values below 0 or clamping results to be minimum 0.

		finalL += L;
	}

	return finalL;
}