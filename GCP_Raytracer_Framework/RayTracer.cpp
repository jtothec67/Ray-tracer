#include "RayTracer.h"

RayTracer::RayTracer()
{
	// Generate hemisphere samples on start
	GenerateHemisphereSamples(mNumAOSamples);
}

glm::vec3 RayTracer::TraceRay(Ray _ray, glm::vec3 _camPos, int _depth)
{
	// Find the closest object hit by the ray
	glm::vec3 currentHitPos;
	RayObject* currentRayObject = nullptr;

	float closestLength = 10000.f;

	// Loop through all rayObject to find which one we hit first
	for (auto rayObject : rayObjects)
	{
		glm::vec3 hitPos;
		if (rayObject->RayIntersect(_ray, hitPos)) // If ray hits something
		{
			glm::vec3 hitToOrigin = _ray.origin - hitPos;
			float length = glm::length(hitToOrigin);

			// See if it's the closest hit
			if (length < closestLength)
			{
				// Update variables with closest info and store the object for later
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
	for (auto& light : mLights)
	{
		float shadowFactor = 1.0f;

		if (mShadows)
		{
			// Check if in shadow
			for (auto rayObject : rayObjects)
			{
				// Skip light objects
				if (rayObject->IsLight())
					continue;

				glm::vec3 hitPos;
				// Offset from surface to avoid self-shadowing
				glm::vec3 shadowRayFrom = currentHitPos + (currentRayObject->NormalAtPosition(currentHitPos) * 0.001f);

				// If ray hits, check it's between hit position and the light
				if (rayObject->RayIntersect(Ray(shadowRayFrom, glm::normalize(light->GetPosition() - currentHitPos)), hitPos))
				{
					if (glm::length(hitPos - currentHitPos) < glm::length(light->GetPosition() - currentHitPos))
					{
						// Adjust shadow factor based on transparency
						shadowFactor *= rayObject->GetTransparency();

						// If fully opaque, break out of the loop
						if (shadowFactor <= 0.0f)
						{
							shadowFactor = 0.0f;
							break;
						}
					}
				}
			}
		}

		if (shadowFactor > 0.0f)
		{
			// Calculate and add light contribution
			glm::vec3 thisPixelCol = currentRayObject->ShadeAtPosition(currentHitPos, light, _camPos, mPBR);
			finalPixelCol += thisPixelCol * shadowFactor;
		}
	}


	if (mAmbientOcclusion)
	{
		// Add ambient occlusion
		float ao = ComputeAO(currentHitPos, currentRayObject->NormalAtPosition(currentHitPos));
		finalPixelCol += mAmbientColour * currentRayObject->GetAlbedo() * (1.0f - mAOStrength + ao * mAOStrength); // Linear interpolation between AO strength and AO
	}
	else
	{
		// Add ambient light
		finalPixelCol += mAmbientColour * currentRayObject->GetAlbedo();
	}

	// Reflections
	// Check if the depth we're at exceeds the max depth, and that the reflectivity is enough to make a difference
	if (_depth < mMaxDepth && currentRayObject->GetReflectivity() > 0.01f)
	{
		// Calculate direction to reflect
		glm::vec3 reflectionDir = glm::reflect(_ray.direction,currentRayObject->NormalAtPosition(currentHitPos));
		// Create reflection ray
		Ray reflectionRay(currentHitPos + currentRayObject->NormalAtPosition(currentHitPos) * 0.001f, reflectionDir);
		// Trace the reflected ray to get the colour (recursion), add 1 to the depth
		glm::vec3 reflectionColor = TraceRay(reflectionRay, _camPos, _depth + 1);
		// Add reflection colour multiplied by reflectivity value of the object
		finalPixelCol = glm::mix(finalPixelCol, reflectionColor, currentRayObject->GetReflectivity());
	}

	// Transparency / refraction
	// (transparency linked with refractions as a non refractive transparent object just refracts the rays to the same direction as they already were)
	if (_depth < mMaxDepth && currentRayObject->GetTransparency() > 0.01f)
	{
		// Calculate refraction direction (refract takes incident direction, normal, and ratio of RI coming from and going in to (we just assume 1 which is RI of air))
		glm::vec3 refractionDir = glm::refract(_ray.direction, currentRayObject->NormalAtPosition(currentHitPos), 1.0f / currentRayObject->GetRefractiveIndex());
		// Create refraction ray
		Ray refractionRay(currentHitPos - currentRayObject->NormalAtPosition(currentHitPos) * 0.001f, refractionDir);
		// Trace the refracted ray to get the colour (recursion), add 1 to the depth
		glm::vec3 refractionColor = TraceRay(refractionRay, _camPos, _depth + 1);
		// Add refraction colour multiplied by transparency value of the object
		finalPixelCol = glm::mix(finalPixelCol, refractionColor, currentRayObject->GetTransparency());
	}

	return finalPixelCol;
}

void RayTracer::SetNumAOSamples(int _numSamples)
{
	if (mNumAOSamples == _numSamples)
		return; // Don't recalculate samples if nothing changed

	// Recalculate samples
	mNumAOSamples = _numSamples;
	GenerateHemisphereSamples(mNumAOSamples);
}

void RayTracer::SetAOSplits(int _numSplits)
{
	if (mAOSplits == _numSplits)
		return;

	mAOSplits = _numSplits;
	GenerateHemisphereSamples(mNumAOSamples);
}

void RayTracer::GenerateHemisphereSamples(int _numSamples)
{
	// Clear old samples and reserve space for new ones
	mSplitHemisphereSamples.clear();
	mSplitHemisphereSamples.resize(mAOSplits);

	for (int i = 0; i < _numSamples; ++i)
	{
		// Calculate u as a fraction of the current sample index
		float u = static_cast<float>(i) / _numSamples;

		float v = 0;
		float p = 0.5f;

		// Generate v using bit-reversal process
		for (int j = i; j > 0; p *= 0.5f, j >>= 1)
		{
			if (j & 1)
				v += p;
		}

		// Calculate spherical coordinates theta and phi
		float theta = 2 * 3.1415f * v;
		float phi = acos(1 - 2 * u);

		// Convert spherical coordinates to Cartesian coordinates
		float x = sin(phi) * cos(theta);
		float y = sin(phi) * sin(theta);
		float z = cos(phi);

		mSplitHemisphereSamples[i % mAOSplits].emplace_back(x, y, z);
	}
}

float RayTracer::ComputeAO(glm::vec3 _intersectPosition, glm::vec3 _normal)
{
	int occlusionCount = 0;
	int totalSamplesChecked = 0;

	// Check if each sample hits anything in each split vector
	for (auto& sampleVector : mSplitHemisphereSamples)
	{
		bool hit = false;
		for (auto& sampleDir : sampleVector)
		{
			totalSamplesChecked++;
			// If our sample is beyond 90 degrees from the normal, flip it
			glm::vec3 orientedSampleDir = glm::dot(sampleDir, _normal) > 0 ? sampleDir : -sampleDir;
			// Create a ray from the intersect position (offset slightly to avoid intersecting with itself) with the samples direction
			Ray sampleRay(_intersectPosition + _normal * 0.001f, orientedSampleDir);

			// Check if the ray hits anything
			for (auto rayObject : rayObjects)
			{
				// Skip light objects
				if (rayObject->IsLight())
					continue;

				glm::vec3 hitPos;
				// If it hits
				if (rayObject->RayIntersect(sampleRay, hitPos))
				{
					//Check if hit is in the AO radius specified
					if (glm::length(hitPos - _intersectPosition) < mAORadius)
					{
						// Increase occlusion count
						occlusionCount++;
						hit = true;
						// Don't need to check more objects so break out of object loop
						break;
					}
				}
			}
		}
		if (!hit)
			return 1.0f - (static_cast<float>(occlusionCount) / totalSamplesChecked);
	}

	return 1.0f - (static_cast<float>(occlusionCount) / totalSamplesChecked);
}