#include "RayObject.h"

glm::vec3 RayObject::ShadeAtPosition(glm::vec3 _intersectPosition, glm::vec3 _lightDir)
{
	glm::vec3 lightDir = glm::normalize(_lightDir);
	glm::vec3 lightCol = glm::vec3(1, 1, 1);
	glm::vec3 L = (glm::dot(lightDir, NormalAtPosition(_intersectPosition))) * mAlbedo * lightCol;

	return L;
}