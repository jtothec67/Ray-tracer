#pragma once

#include "RayObject.h"
#include "Light.h"

#include <vector>

class RayTracer
{
public:
	RayTracer() {}
	~RayTracer() {}

	glm::vec3 TraceRay(Ray _ray, glm::vec3 _camPos);

	std::vector<glm::vec3> GenerateHemisphereSamples(int _numSamples);
	float ComputeAO(glm::vec3 _position, glm::vec3 _normal, int _numSamples);

	void SetLights(std::vector<Light>* _lights) { mLights = _lights; }

	glm::vec3 mBackgroundColour{ 0, 0, 0.2f };

	glm::vec3 mAmbientColour{ 0.05f, 0.05f, 0.05f };

	bool mAmbientOcclusion = true;
	float mAOStrength = 1.f;
	float mAORadius = 5.f;
	int mNumAOSamples = 16;

	bool mShadows = true;

	std::vector<RayObject*> rayObjects;
	std::vector<Light>* mLights;
};