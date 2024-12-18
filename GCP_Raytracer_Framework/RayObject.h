#pragma once

#include "Ray.h"
#include "Light.h"

#include <GLM/glm.hpp>

#include <string>
#include <vector>

class RayObject
{
public:
	RayObject() {}
	~RayObject() {}

	// Pure virtual functions for derived classes to implement
	virtual bool RayIntersect(Ray _ray, glm::vec3& _intersectPosition) = 0;
	virtual glm::vec3 NormalAtPosition(glm::vec3 _intersectPosition) = 0;

	glm::vec3 ShadeAtPosition(glm::vec3 _intersectPosition, Light *_light, glm::vec3 _camPos, bool _pbr);

	glm::vec3 CalulateDiffuseAndSpecular(glm::vec3 _intersectPosition, Light* _light, glm::vec3 _camPos);
	glm::vec3 CalculatePBR(glm::vec3 _intersectPosition, Light* _light, glm::vec3 _camPos);

	// PBR functions
	glm::vec3 fresnelSchlick(float cosTheta, const glm::vec3& F0);
	float DistributionGGX(const glm::vec3& N, const glm::vec3& H, float roughness);
	float GeometrySchlickGGX(float NdotV, float roughness);
	float GeometrySmith(const glm::vec3& N, const glm::vec3& V, const glm::vec3& L, float roughness);

	// Getters and Setters
	glm::vec3 GetPosition() { return mPosition; }
	glm::vec3 GetAlbedo() { return mAlbedo; }
	float GetShininess() { return mShininess; }
	float GetReflectivity() { return mReflectivity; }
	float GetRefractiveIndex() { return mRefractiveIndex; }
	float GetTransparency() { return mTransparency; }
	float GetMetallic() { return mMetallic; }
	float GetRoughness() { return mRoughness; }
	bool IsLight() { return mIsLight; }

	void SetPosition(glm::vec3 _position) { mPosition = _position; }
	void SetAlbedo(glm::vec3 _albedo) { mAlbedo = _albedo; }
	void SetShininess(float _shininess) { mShininess = _shininess; }
	void SetReflectivity(float _reflectivity) { mReflectivity = _reflectivity; }
	void SetRefractiveIndex(float _refractiveIndex) { mRefractiveIndex = _refractiveIndex; }
	void SetTransparency(float _transparency) { mTransparency = _transparency; }
	void SetMetallic(float _metallic) { mMetallic = _metallic; }
	void SetRoughness(float _roughness) { mRoughness = _roughness; }
	void IsLight(bool _isLight) { mIsLight = _isLight; }

	// UI function
	virtual void UpdateUI();

protected:
	std::string mName = "Object";

	glm::vec3 mPosition{ 0 };
	glm::vec3 mAlbedo{ 0 };
	float mShininess = 1;

	float mReflectivity = 0;

	float mRefractiveIndex = 1.0f;
	float mTransparency = 0.0f;

	float mMetallic = 0.f;
	float mRoughness = 1.f;

	bool mIsLight = false;
};