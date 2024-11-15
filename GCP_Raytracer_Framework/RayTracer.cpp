#include "RayTracer.h"
#include <iostream>

glm::vec3 RayTracer::TraceRay(Ray _ray, glm::vec3 _camPos)
{
	glm::vec3 currentHitPos;
	RayObject* currentRayObject = nullptr;

	float closestLength = 10000.f;

	// Loop through all rayObject to find which one we hit first
	for (auto rayObject : rayObjects)
	{
		glm::vec3 hitPos;
		if (rayObject->RayIntersect(_ray, hitPos))
		{
			glm::vec3 hitToOrigin = _ray.origin - hitPos;
			float length = glm::length(hitToOrigin);

			if (length < closestLength)
			{
				currentRayObject = rayObject;
				currentHitPos = hitPos;
				closestLength = length;
			}
		}
	}

	// If we didn't hit anything, return the background colour
	if (currentRayObject == nullptr)
		return mBackgroundColour;

	glm::vec3 finalPixelCol = glm::vec3(0.f, 0.f, 0.f);

	// Loop through all of the lights and add their contribution to the final pixel colour
	for (auto &light : *mLights)
	{
		glm::vec3 thisPixelCol = currentRayObject->ShadeAtPosition(currentHitPos, glm::normalize(light.position - currentHitPos), light.colour, _camPos, light.position);

		if (mShadows)
		{
			// Check if in shadow
			for (auto rayObject : rayObjects)
			{
				if (rayObject->mIsLight)
					continue;

				glm::vec3 hitPos;
				glm::vec3 shadowRayFrom = currentHitPos + (currentRayObject->NormalAtPosition(currentHitPos) * 0.01f); // Offset from surface to avoid self-shadowing

				// If ray hits, check it's between hit position and the light
				if (rayObject->RayIntersect(Ray(shadowRayFrom, glm::normalize(light.position - currentHitPos)), hitPos))
				{
					if (glm::length(hitPos - currentHitPos) > glm::length(light.position - currentHitPos))
						continue;

					// Shadow
					thisPixelCol = glm::vec3(0, 0, 0);
				}
			}
		}
		
		finalPixelCol += thisPixelCol;
	}

	if (mAmbientOcclusion)
	{
		// Add ambient occlusion

		glm::vec3 normal = currentRayObject->NormalAtPosition(currentHitPos);
		float ao = ComputeAO(currentHitPos, normal, mNumAOSamples) * mAOStrength;
		finalPixelCol += mAmbientColour * currentRayObject->mAlbedo * ao;
	}
	else
	{
		// Add ambient light
		finalPixelCol += mAmbientColour * currentRayObject->mAlbedo;
	}

	return finalPixelCol;
}

std::vector<glm::vec3> RayTracer::GenerateHemisphereSamples(int _numSamples)
{
	std::vector<glm::vec3> samples;
	samples.reserve(_numSamples);

	for (int i = 0; i < _numSamples; ++i)
	{
		float u = static_cast<float>(i) / _numSamples;
		float v = static_cast<float>(i) / _numSamples;
		float theta = 2 * 3.1415 * u;
		float phi = acos(2 * v - 1);

		float x = sin(phi) * cos(theta);
		float y = sin(phi) * sin(theta);
		float z = cos(phi);

		samples.emplace_back(x, y, z);
	}

	return samples;
}

float RayTracer::ComputeAO(glm::vec3 _intersectPosition, glm::vec3 _normal, int _numSamples)
{
	int occlusionCount = 0;
	auto samples = GenerateHemisphereSamples(_numSamples);

	for (const auto& sampleDir : samples)
	{
		glm::vec3 orientedSampleDir = glm::dot(sampleDir, _normal) > 0 ? sampleDir : -sampleDir;
		Ray sampleRay(_intersectPosition + _normal * 0.01f, orientedSampleDir);

		for (auto rayObject : rayObjects)
		{
			if (rayObject->mIsLight)
				continue;

			glm::vec3 hitPos;
			if (rayObject->RayIntersect(sampleRay, hitPos))
			{
				if (glm::length(hitPos - _intersectPosition) < mAORadius)

				occlusionCount++;
				break;
			}
		}


	}
	return 1.0f - (static_cast<float>(occlusionCount) / _numSamples);
}