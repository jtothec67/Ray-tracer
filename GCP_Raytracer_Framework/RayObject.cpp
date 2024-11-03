#include "RayObject.h"

#include <iostream>

glm::vec3 RayObject::ShadeAtPosition(glm::vec3 _intersectPosition, glm::vec3 _lightDir, glm::vec3 _lightCol, glm::vec3 _camPos)
{
	if (mIsLight)
		return mAlbedo;

	glm::vec3 diffuse = (glm::max(glm::dot(_lightDir, NormalAtPosition(_intersectPosition)), 0.f)) * mAlbedo * _lightCol;

	glm::vec3 reflectDir = glm::reflect(-_lightDir, NormalAtPosition(_intersectPosition));
	float spec = glm::pow(glm::max(glm::dot(glm::normalize(_camPos - _intersectPosition), reflectDir), 0.f), 32);
	glm::vec3 specular = spec * mShininess * _lightCol;

	return diffuse + specular;
}