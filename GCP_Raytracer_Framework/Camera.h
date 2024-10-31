#pragma once

#include "Ray.h"

#include <GLM/glm.hpp>

class Camera
{
public:

	Camera(glm::vec3 _position,glm::vec3 _rotation, glm::ivec2 _winSize);
	~Camera() {}

	Ray GetRay(glm::ivec2 _windowPos, glm::ivec2 _windowSize);

	glm::vec3 position{ 0.f };
	glm::vec3 rotation{ 0.f };

	glm::mat4 viewMat{ 1.f };
	glm::mat4 projectionMat{ 1.f };
};