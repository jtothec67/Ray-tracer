#include "RayObject.h"

#include <iostream>

glm::vec3 RayObject::ShadeAtPosition(glm::vec3 _intersectPosition, glm::vec3 _lightDir, glm::vec3 _lightCol, glm::vec3 _camPos)
{
	if (mIsLight)
		return mAlbedo;

	//return CalulateDiffuseAndSpecular(_intersectPosition, _lightDir, _lightCol, _camPos);
	return CalculatePBR(_intersectPosition, _lightDir, _lightCol, _camPos);
}

glm::vec3 RayObject::CalulateDiffuseAndSpecular(glm::vec3 _intersectPosition, glm::vec3 _lightDir, glm::vec3 _lightCol, glm::vec3 _camPos)
{
	glm::vec3 normal = NormalAtPosition(_intersectPosition);
	glm::vec3 eyeDir = glm::normalize(_camPos - _intersectPosition);

	glm::vec3 diffuse = (glm::max(glm::dot(normal, _lightDir), 0.f)) * mAlbedo * _lightCol;

	glm::vec3 reflectDir = glm::reflect(-_lightDir, normal);
	float spec = glm::pow(glm::max(glm::dot(eyeDir, reflectDir), 0.f), 32);
	glm::vec3 specular = spec * mShininess * _lightCol;

	return diffuse + specular;
}

glm::vec3 RayObject::CalculatePBR(glm::vec3 _intersectPosition, glm::vec3 _lightDir, glm::vec3 _lightCol, glm::vec3 _camPos)
{
	glm::vec3 normal = NormalAtPosition(_intersectPosition);
	glm::vec3 eyeDir = glm::normalize(_camPos - _intersectPosition);

	glm::vec3 lightDir = glm::normalize(_lightDir);

	glm::vec3 halfVector = glm::normalize(lightDir + eyeDir);

	float NdotL = glm::max(glm::dot(normal, lightDir), 0.0f);
	float NdotH = glm::max(glm::dot(normal, halfVector), 0.0f);
	float VdotH = glm::max(glm::dot(eyeDir, halfVector), 0.0f);

	float roughness = mRoughness;
	float F0 = 0.04f;
	glm::vec3 F = glm::vec3(F0) + (mAlbedo - glm::vec3(F0)) * glm::pow(1.0f - VdotH, 5.0f);

	float NDF = (roughness + 1) * glm::pow(NdotH, roughness) / (2.0f * 3.1415 * glm::pow(1.0f + (roughness * roughness - 1.0f) * NdotH * NdotH, 2.0f));
	float G = glm::min(1.0f, glm::min(2.0f * NdotH * NdotL / VdotH, 2.0f * NdotH * glm::dot(normal, eyeDir) / VdotH));

	glm::vec3 kS = F;
	glm::vec3 kD = glm::vec3(1.0f) - kS;
	kD *= 1.0f - mMetallic;

	glm::vec3 numerator = NDF * G * F;
	float denominator = 4.0f * glm::max(4.0f * NdotL * NdotH, 0.001f);
	glm::vec3 specular = numerator / denominator;

	glm::vec3 ambient = glm::vec3(0.03f) * mAlbedo * _lightCol;

	glm::vec3 colour = (kD * mAlbedo / 3.1415f) + specular;

	return (colour * NdotL + ambient) * _lightCol;
}