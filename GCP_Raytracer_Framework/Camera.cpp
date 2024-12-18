#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 _position, glm::vec3 _rotation, glm::ivec2 _winSize)
{
	position = _position;
	rotation = _rotation;

	lastWinSize = _winSize;

	
	CalculateMatricies(_winSize);
}

void Camera::CalculateMatricies(glm::ivec2 _winSize)
{
	viewMat = glm::mat4{ 1.f };
	
	viewMat = glm::rotate(viewMat, glm::radians(rotation.x), glm::vec3(1, 0, 0));
	viewMat = glm::rotate(viewMat, glm::radians(rotation.y), glm::vec3(0, 1, 0));
	viewMat = glm::rotate(viewMat, glm::radians(rotation.z), glm::vec3(0, 0, 1));
	viewMat = glm::translate(viewMat, position);
	
	projectionMat = glm::mat4{ 1.f };
	projectionMat = glm::perspective(glm::radians(60.f), (float)_winSize.x / (float)_winSize.y, 0.1f, 100.f);
}

// This function generates a ray from the camera through a point on the screen
Ray Camera::GetRay(glm::ivec2 _windowPos, glm::ivec2 _windowSize)
{
    // Update the last window size
    lastWinSize = _windowSize;

    // Convert window position and size to floating point vectors
    glm::vec2 fWinPos = glm::vec2((float)_windowPos.x, (float)_windowPos.y);
    glm::vec2 fWinSize = glm::vec2((float)_windowSize.x, (float)_windowSize.y);

    // Initialize the near point in normalized device coordinates
    glm::vec4 nearPoint{ 0 };
    nearPoint.x = fWinPos.x / fWinSize.x;
    nearPoint.y = fWinPos.y / fWinSize.y;

    // Adjust the near point to range [-1, 1]
    nearPoint = nearPoint - 0.5f;
    nearPoint = nearPoint * 2.0f;

    nearPoint.z = -1;
    nearPoint.w = 1;

    // Initialize the far point in normalized device coordinates
    glm::vec4 farPoint = nearPoint;
    farPoint.z = 1;

    // Transform the near and far points from clip space to view space
    nearPoint = glm::inverse(projectionMat) * nearPoint;
    farPoint = glm::inverse(projectionMat) * farPoint;

    // Normalize the points by dividing by w
    nearPoint = nearPoint / nearPoint.w;
    farPoint = farPoint / farPoint.w;

    // Transform the points from view space to world space
    nearPoint = glm::inverse(viewMat) * nearPoint;
    farPoint = glm::inverse(viewMat) * farPoint;

    // Create the ray with origin at the near point and direction towards the far point
    Ray ray;
    ray.origin = glm::vec3(nearPoint);
    glm::vec3 direction = glm::normalize(farPoint - nearPoint);
    ray.direction = direction;

    return ray;
}

// Helper functions to get the camera's forward, right, and up vectors
glm::vec3 Camera::GetForward()
{
	glm::vec3 forward = glm::vec3(viewMat[0][2], viewMat[1][2], viewMat[2][2]);
	return glm::normalize(forward);
}

glm::vec3 Camera::GetRight()
{
	glm::vec3 right = glm::vec3(viewMat[0][0], viewMat[1][0], viewMat[2][0]);
	return glm::normalize(right);
}

glm::vec3 Camera::GetUp()
{
	glm::vec3 up = glm::vec3(viewMat[0][1], viewMat[1][1], viewMat[2][1]);
	return glm::normalize(up);
}