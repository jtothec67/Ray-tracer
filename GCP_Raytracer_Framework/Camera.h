#pragma once

#include "Ray.h"

#include <GLM/glm.hpp>

class Camera
{
public:

	Camera(glm::vec3 _position, glm::ivec2 _winSize);
	~Camera() {}

	Ray GetRay(glm::ivec2 _windowPos, glm::ivec2 _windowSize);

	glm::mat4 mView{ 1.f };
	glm::mat4 mProjection{ 1.f };
};