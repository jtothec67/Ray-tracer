#include "Cylinder.h"

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>

Cylinder::Cylinder(std::string _name, glm::vec3 _position, float _radius, float _height)
{
	mName = _name;
	mPosition = _position;
	mRadius = _radius;
	mHeight = _height;
}

Cylinder::Cylinder(std::string _name, glm::vec3 _position, float _radius, float _height, glm::vec3 _albedo)
{
	mName = _name;
	mPosition = _position;
	mRadius = _radius;
	mHeight = _height;
	mAlbedo = _albedo;
}

bool Cylinder::RayIntersect(Ray _ray, glm::vec3& _intersectPosition)
{
    // Ray origin and direction
    glm::vec3 rayOrigin = _ray.origin;
    glm::vec3 rayDirection = glm::normalize(_ray.direction);

    // Cylinder properties
    glm::vec3 cylinderAxis = glm::normalize(mAxis);
    glm::vec3 cylinderBase = mPosition;
    float radius = mRadius;
    float height = mHeight;

    // Check if the ray origin is inside the cylinder
    glm::vec3 deltaP = rayOrigin - cylinderBase;
    float projectionLength = glm::dot(deltaP, cylinderAxis);
    glm::vec3 closestPointOnAxis = cylinderBase + projectionLength * cylinderAxis;
    float distanceToAxis = glm::length(rayOrigin - closestPointOnAxis);

    if (distanceToAxis < radius && projectionLength >= 0 && projectionLength <= height)
    {
        return false;
    }

    // Calculate the quadratic coefficients for the infinite cylinder
    glm::vec3 d = rayDirection - glm::dot(rayDirection, cylinderAxis) * cylinderAxis;
    glm::vec3 deltaPProj = deltaP - glm::dot(deltaP, cylinderAxis) * cylinderAxis;

    float a = glm::dot(d, d);
    float b = 2.0f * glm::dot(d, deltaPProj);
    float c = glm::dot(deltaPProj, deltaPProj) - radius * radius;

    // Solve the quadratic equation
    float discriminant = b * b - 4 * a * c;
    float t = -1.0f;
    if (discriminant >= 0)
    {
        float sqrtDiscriminant = std::sqrt(discriminant);
        float t1 = (-b - sqrtDiscriminant) / (2 * a);
        float t2 = (-b + sqrtDiscriminant) / (2 * a);

        // Find the nearest valid intersection
        t = t1;
        if (t < 0 || (t2 < t && t2 >= 0))
        {
            t = t2;
        }

        if (t >= 0)
        {
            // Calculate the intersection position
            glm::vec3 intersectPos = rayOrigin + t * rayDirection;

            // Check if the intersection is within the cylinder's height
            float y = glm::dot(intersectPos - cylinderBase, cylinderAxis);
            if (y >= 0 && y <= height)
            {
                _intersectPosition = intersectPos;
                return true;
            }
        }
    }

    // Check for intersection with the top and bottom caps
    glm::vec3 topCenter = cylinderBase + height * cylinderAxis;
    float tCap;

    // Bottom cap
    tCap = glm::dot(cylinderBase - rayOrigin, cylinderAxis) / glm::dot(rayDirection, cylinderAxis);
    if (tCap >= 0)
    {
        glm::vec3 intersectPos = rayOrigin + tCap * rayDirection;
        if (glm::length(intersectPos - cylinderBase) <= radius)
        {
            _intersectPosition = intersectPos;
            return true;
        }
    }

    // Top cap
    tCap = glm::dot(topCenter - rayOrigin, cylinderAxis) / glm::dot(rayDirection, cylinderAxis);
    if (tCap >= 0)
    {
        glm::vec3 intersectPos = rayOrigin + tCap * rayDirection;
        if (glm::length(intersectPos - topCenter) <= radius)
        {
            _intersectPosition = intersectPos;
            return true;
        }
    }

    return false;
}

glm::vec3 Cylinder::NormalAtPosition(glm::vec3 _intersectPosition)
{
    // Cylinder properties
    glm::vec3 cylinderAxis = glm::normalize(mAxis);
    glm::vec3 cylinderBase = mPosition;
    float height = mHeight;

    // Calculate the vector from the base of the cylinder to the position
    glm::vec3 deltaP = _intersectPosition - cylinderBase;

    // Project deltaP onto the cylinder axis to find the closest point on the axis
    float projectionLength = glm::dot(deltaP, cylinderAxis);

    // Check if the position is on the top or bottom cap
    if (projectionLength <= 0.001f) // Bottom cap
    {
        return -cylinderAxis;
    }
    else if (projectionLength >= height - 0.001f) // Top cap
    {
        return cylinderAxis;
    }

    // The normal is the vector from the closest point on the axis to the position, normalized
    glm::vec3 closestPointOnAxis = cylinderBase + projectionLength * cylinderAxis;
    glm::vec3 normal = glm::normalize(_intersectPosition - closestPointOnAxis);

    return normal;
}

void Cylinder::UpdateUI()
{
	if (mIsLight)
		return;

	RayObject::UpdateUI();

	ImGui::SliderFloat(("Radius" + mName).c_str(), &mRadius, 0.0f, 20.0f);
	ImGui::SliderFloat(("Height" + mName).c_str(), &mHeight, 0.0f, 20.0f);
	ImGui::SliderFloat3(("Rotation" + mName).c_str(), &mAxis[0], -1.0f, 1.0f);
}