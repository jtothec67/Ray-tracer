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
	void SetNumAOSamples(int _numSamples);
	float ComputeAO(glm::vec3 _position, glm::vec3 _normal);

	void SetBackgroundColour(glm::vec3 _colour) { mBackgroundColour = _colour; }
	void SetAmbientColour(glm::vec3 _colour) { mAmbientColour = _colour; }
	void SetPBR(bool _pbr) { mPBR = _pbr; }
	void SetOptimisedAO(bool _optimisedAO);
	void SetAmbientOcclusion(bool _ao) { mAmbientOcclusion = _ao; }
	void SetAOStrength(float _strength) { mAOStrength = _strength; }
	void SetAORadius(float _radius) { mAORadius = _radius; }
	void SetShadows(bool _shadows) { mShadows = _shadows; }
	void SetMaxDepth(int _depth) { mMaxDepth = _depth; }

	bool GetPBR() { return mPBR; }
	bool GetShadows() { return mShadows; }
	bool GetAmbientOcclusion() { return mAmbientOcclusion; }
	bool GetOptimisedAO() { return mOptimisedAO; }
	glm::vec3 GetAmbientColour() { return mAmbientColour; }
	float GetAOStrength() { return mAOStrength; }
	float GetAORadius() { return mAORadius; }
	int GetNumAOSamples() { return mNumAOSamples; }
	int GetMaxDepth() { return mMaxDepth; }

	const std::vector<Light*>& GetLights() { return mLights; }
	const std::vector<RayObject*>& GetRayObjects() { return rayObjects; }


	void AddRayObject(RayObject* _rayObject) { rayObjects.push_back(_rayObject); }
	void AddLight(Light* _light) { mLights.push_back(_light); }

	int GetSizeOfRayObjects() { return rayObjects.size(); }
	int GetSizeOfLights() { return mLights.size(); }
	void ClearScene() { rayObjects.clear(); mLights.clear(); }

private:
	glm::vec3 mBackgroundColour{ 0, 0, 0.2f };

	glm::vec3 mAmbientColour{ 0.08f, 0.08f, 0.08f };

	bool mPBR = true;

	bool mOptimisedAO = true;

	bool mAmbientOcclusion = false;
	float mAOStrength = 1.f;
	float mAORadius = 5.f;
	int mNumAOSamples = 16;

	bool mShadows = true;

	int mMaxDepth = 2;

	std::vector<glm::vec3> mInitialCheckHemisphereSamples;
	std::vector<glm::vec3> mRestOfHemisphereSamples;

	std::vector<glm::vec3> mHemisphereSamples;

	std::vector<RayObject*> rayObjects;
	std::vector<Light*> mLights;
};