#pragma once

#include "RayObject.h"
#include "Light.h"

#include <vector>

class RayTracer
{
public:
	RayTracer();
	~RayTracer() {}

	glm::vec3 TraceRay(Ray _ray, glm::vec3 _camPos, int _depth);

	void GenerateHemisphereSamples(int _numSamples);
	void SetNumSamples(int _numSamples);

	float ComputeAO(glm::vec3 _position, glm::vec3 _normal);

	void SetLights(std::vector<Light>* _lights) { mLights = _lights; }

	glm::vec3 mBackgroundColour{ 0, 0, 0.2f };

	glm::vec3 mAmbientColour{ 0.05f, 0.05f, 0.05f };

	bool mPBR = true;

	bool mAmbientOcclusion = false;
	float mAOStrength = 1.f;
	float mAORadius = 5.f;
	int mNumAOSamples = 16;

	bool mShadows = true;

	int mMaxDepth = 2;

	std::vector<glm::vec3> mHemisphereSamples;

	std::vector<RayObject*> rayObjects;
	std::vector<Light>* mLights;
};