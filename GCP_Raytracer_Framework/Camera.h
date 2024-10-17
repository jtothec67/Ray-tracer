#pragma once

#include "Ray.h"

#include <GLM/glm.hpp>

class Camera
{
public:

	Camera() {}
	~Camera() {}

	Ray GetRay(glm::ivec2 _windowPos);

	glm::mat4 mView{ 0 };
	glm::mat4 mProjection{ 0 };
};