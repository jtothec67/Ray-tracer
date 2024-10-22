#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Camera::Camera(glm::vec3 _position, glm::ivec2 _winSize)
{
	mView = glm::translate(mView, _position);
	mView = glm::inverse(mView);

	mProjection = glm::perspective(glm::radians(60.f), (float)_winSize.x / (float)_winSize.y, 0.1f, 100.f);
}

Ray Camera::GetRay(glm::ivec2 _windowPos, glm::ivec2 _windowSize)
{
	glm::vec2 fWinPos = glm::vec2((float)_windowPos.x, (float)_windowPos.y);
	glm::vec2 fWinSize = glm::vec2((float)_windowSize.x, (float)_windowSize.y);

	glm::vec4 nearPoint{ 0 };
	nearPoint.x = fWinPos.x / fWinSize.x;
	nearPoint.y = fWinPos.y / fWinSize.y;

	nearPoint = nearPoint - 0.5f;
	nearPoint = nearPoint * 2.0f;

	nearPoint.z = -1;
	nearPoint.w = 1;

	glm::vec4 farPoint = nearPoint;
	farPoint.z = 1;

	nearPoint = glm::inverse(mProjection) * nearPoint;
	farPoint = glm::inverse(mProjection) * farPoint;

	nearPoint = nearPoint / nearPoint.w;
	farPoint = farPoint / farPoint.w;

	nearPoint = mView * nearPoint;
	farPoint = mView * farPoint;

	Ray ray;
	ray.origin = glm::vec3(nearPoint);
	glm::vec3 direction = glm::normalize(farPoint - nearPoint);
	ray.direction = direction;

	return ray;
}