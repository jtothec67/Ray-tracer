#include "Box.h"

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

Box::Box(std::string _name, glm::vec3 _position, glm::vec3 _size)
{
	mName = _name;
	mPosition = _position;
	mSize = _size;
}

Box::Box(std::string _name, glm::vec3 _position, glm::vec3 _size, glm::vec3 _albedo)
{
	mName = _name;
	mPosition = _position;
	mSize = _size;
	mAlbedo = _albedo;
}

bool Box::RayIntersect(Ray _ray, glm::vec3& _intersectPosition)
{
	glm::vec3 axis = glm::normalize(mAxis);

    // Calculate the rotation matrix from the direction vector
    glm::vec3 defaultAxis(0, 1, 0); // Assuming the box is initially aligned along the z-axis
    glm::quat rotationQuat = glm::rotation(defaultAxis, axis);
    glm::mat4 rotationMatrix = glm::toMat4(rotationQuat);
    glm::mat4 invRotationMatrix = glm::inverse(rotationMatrix);

    glm::vec3 localRayOrigin = glm::vec3(invRotationMatrix * glm::vec4(_ray.origin - mPosition, 1.0f));
    glm::vec3 localRayDirection = glm::normalize(glm::vec3(invRotationMatrix * glm::vec4(_ray.direction, 0.0f)));

    // Check if the ray starts inside the box
    glm::vec3 minBound = glm::vec3(0.0f);
    glm::vec3 maxBound = mSize;
    if (localRayOrigin.x >= minBound.x && localRayOrigin.x <= maxBound.x &&
        localRayOrigin.y >= minBound.y && localRayOrigin.y <= maxBound.y &&
        localRayOrigin.z >= minBound.z && localRayOrigin.z <= maxBound.z)
    {
        return false;
    }

    // Calculate intersection with the box's planes
    glm::vec3 tMin = (minBound - localRayOrigin) / localRayDirection;
    glm::vec3 tMax = (maxBound - localRayOrigin) / localRayDirection;
    glm::vec3 t1 = glm::min(tMin, tMax);
    glm::vec3 t2 = glm::max(tMin, tMax);
    float tNear = std::max(std::max(t1.x, t1.y), t1.z);
    float tFar = std::min(std::min(t2.x, t2.y), t2.z);

    if (tNear > tFar || tFar < 0)
    {
        return false;
    }

    // Calculate the intersection position in world space
    glm::vec3 localIntersectPosition = localRayOrigin + tNear * localRayDirection;
    _intersectPosition = glm::vec3(rotationMatrix * glm::vec4(localIntersectPosition, 1.0f)) + mPosition;

    return true;
}

glm::vec3 Box::NormalAtPosition(glm::vec3 _intersectPosition)
{
    glm::vec3 axis = glm::normalize(mAxis);

    // Calculate the rotation matrix from the direction vector
    glm::vec3 defaultAxis(0, 1, 0); // Assuming the box is initially aligned along the y-axis
    glm::quat rotationQuat = glm::rotation(defaultAxis, axis);
    glm::mat4 rotationMatrix = glm::toMat4(rotationQuat);
    glm::mat4 invRotationMatrix = glm::inverse(rotationMatrix);

    // Transform the intersection position to the box's local space
    glm::vec3 localIntersectPosition = glm::vec3(invRotationMatrix * glm::vec4(_intersectPosition - mPosition, 1.0f));

    // Determine the normal in local space
    glm::vec3 localNormal;
    glm::vec3 center = mSize * 0.5f;
    glm::vec3 localCenterIntersectPosition = localIntersectPosition - center;

    if (glm::abs(localCenterIntersectPosition.x) > glm::abs(localCenterIntersectPosition.y) &&
        glm::abs(localCenterIntersectPosition.x) > glm::abs(localCenterIntersectPosition.z))
    {
        localNormal = glm::vec3(localCenterIntersectPosition.x > 0 ? 1 : -1, 0, 0);
    }
    else if (glm::abs(localCenterIntersectPosition.y) > glm::abs(localCenterIntersectPosition.x) &&
        glm::abs(localCenterIntersectPosition.y) > glm::abs(localCenterIntersectPosition.z))
    {
        localNormal = glm::vec3(0, localCenterIntersectPosition.y > 0 ? 1 : -1, 0);
    }
    else
    {
        localNormal = glm::vec3(0, 0, localCenterIntersectPosition.z > 0 ? 1 : -1);
    }

    // Transform the normal back to world space
    glm::vec3 worldNormal = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(localNormal, 0.0f)));
    return worldNormal;
}

void Box::UpdateUI()
{
    if (mIsLight)
        return;

    RayObject::UpdateUI();

    ImGui::SliderFloat3(("Size " + mName).c_str(), &mSize[0], 0.0f, 20.0f);
    ImGui::SliderFloat3(("Rotation " + mName).c_str(), &mAxis[0], -1.0f, 1.0f);
}