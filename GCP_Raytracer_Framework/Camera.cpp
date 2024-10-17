#include "Camera.h"

Ray Camera::GetRay(glm::ivec2 _windowPos)
{
	Ray ray;
	ray.origin = glm::vec3(_windowPos.x, _windowPos.y, 0);
	ray.direction = glm::vec3(0, 0, -1);

	return ray;
}