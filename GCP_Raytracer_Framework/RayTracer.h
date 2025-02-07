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

	// Ambient Occlusion
	void GenerateHemisphereSamples(int _numSamples);
	void SetNumAOSamples(int _numSamples);
	float ComputeAO(glm::vec3 _position, glm::vec3 _normal);

	// Getters and Setters
	void SetBackgroundColour(glm::vec3 _colour) { mBackgroundColour = _colour; }
	void SetAmbientColour(glm::vec3 _colour) { mAmbientColour = _colour; }
	void SetPBR(bool _pbr) { mPBR = _pbr; }
	void SetAmbientOcclusion(bool _ao) { mAmbientOcclusion = _ao; }
	void SetAOStrength(float _strength) { mAOStrength = _strength; }
	void SetAORadius(float _radius) { mAORadius = _radius; }
	void SetShadows(bool _shadows) { mShadows = _shadows; }
	void SetMaxDepth(int _depth) { mMaxDepth = _depth; }
	void SetAOSplits(int _numSplits);

	bool GetPBR() { return mPBR; }
	bool GetShadows() { return mShadows; }
	bool GetAmbientOcclusion() { return mAmbientOcclusion; }
	glm::vec3 GetAmbientColour() { return mAmbientColour; }
	float GetAOStrength() { return mAOStrength; }
	float GetAORadius() { return mAORadius; }
	int GetNumAOSamples() { return mNumAOSamples; }
	int GetMaxDepth() { return mMaxDepth; }
	int GetAOSplits() { return mAOSplits; }

	const std::vector<Light*>& GetLights() { return mLights; }
	const std::vector<RayObject*>& GetRayObjects() { return rayObjects; }

	// Add objects to the scene
	void AddRayObject(RayObject* _rayObject) { rayObjects.push_back(_rayObject); }
	void AddLight(Light* _light) { mLights.push_back(_light); }

	// Scene management
	int GetSizeOfRayObjects() { return rayObjects.size(); }
	int GetSizeOfLights() { return mLights.size(); }
	void ClearScene() { rayObjects.clear(); mLights.clear(); }

private:
	glm::vec3 mBackgroundColour{ 0.2f, 0.2f, 0.2f };
	glm::vec3 mAmbientColour{ 0.08f, 0.08f, 0.08f };
	bool mPBR = true;
	bool mAmbientOcclusion = false;
	float mAOStrength = 1.f;
	float mAORadius = 5.f;
	int mNumAOSamples = 16;
	int mAOSplits = 2;
	bool mShadows = true;
	int mMaxDepth = 2;

	std::vector<std::vector<glm::vec3>> mSplitHemisphereSamples;

	std::vector<RayObject*> rayObjects;
	std::vector<Light*> mLights;
};