#pragma once

#include <GLM/glm.hpp>

struct Light
{
	Light(glm::vec3 _position, glm::vec3 _colour) : position(_position), colour(_colour) {}

	glm::vec3 position{ 0 };
	glm::vec3 colour{ 0 };
};