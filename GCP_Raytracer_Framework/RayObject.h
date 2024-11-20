#pragma once

#include "Ray.h"
#include "Light.h"

#include <GLM/glm.hpp>

#include <vector>

class RayObject
{
public:
	RayObject() {}
	~RayObject() {}

	virtual bool RayIntersect(Ray _ray, glm::vec3& _intersectPosition) = 0;
	virtual glm::vec3 NormalAtPosition(glm::vec3 _intersectPosition) = 0;

	glm::vec3 ShadeAtPosition(glm::vec3 _intersectPosition, glm::vec3 lightDir, glm::vec3 _lightCol, glm::vec3 _camPos, glm::vec3 _lightPos, bool _pbr);

	glm::vec3 CalulateDiffuseAndSpecular(glm::vec3 _intersectPosition, glm::vec3 _lightDir, glm::vec3 _lightCol, glm::vec3 _camPos);
	glm::vec3 CalculatePBR(glm::vec3 _intersectPosition, glm::vec3 _lightDir, glm::vec3 _lightCol, glm::vec3 _camPos, glm::vec3 _lightPos);

	glm::vec3 fresnelSchlick(float cosTheta, const glm::vec3& F0);
	float DistributionGGX(const glm::vec3& N, const glm::vec3& H, float roughness);
	float GeometrySchlickGGX(float NdotV, float roughness);
	float GeometrySmith(const glm::vec3& N, const glm::vec3& V, const glm::vec3& L, float roughness);

	glm::vec3 mPosition{ 0 };
	glm::vec3 mAlbedo{ 0 };
	float mShininess = 1;

	float mReflectivity = 0;

	float mMetallic = 0.5f;
	float mRoughness = 0.1f;

	bool mIsLight = false;
};