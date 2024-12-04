#include "Cylinder.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
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
    glm::vec3 axis = glm::normalize(mAxis);

    // Check if the ray starts inside the cylinder
    glm::vec3 toRayOrigin = _ray.origin - mPosition;
    float projection = glm::dot(toRayOrigin, axis);
    glm::vec3 projectedPoint = mPosition + projection * axis;
    float distanceToAxis = glm::length(_ray.origin - projectedPoint);

    if (distanceToAxis < mRadius && projection >= 0.0f && projection <= mHeight)
    {
        return false; // Ray starts inside the cylinder
    }

    // Transform the ray to the cylinder's local space
    glm::vec3 d = _ray.direction - glm::dot(_ray.direction, axis) * axis;
    glm::vec3 o = _ray.origin - mPosition - glm::dot(_ray.origin - mPosition, axis) * axis;

    float A = glm::dot(d, d);
    float B = 2.0f * glm::dot(d, o);
    float C = glm::dot(o, o) - mRadius * mRadius;

    float discriminant = B * B - 4.0f * A * C;

    float tSide = -1.0f;
    if (discriminant >= 0.0f)
    {
        float sqrtDiscriminant = std::sqrt(discriminant);
        float t1 = (-B - sqrtDiscriminant) / (2.0f * A);
        float t2 = (-B + sqrtDiscriminant) / (2.0f * A);

        if (t1 > t2) std::swap(t1, t2);

        float t = t1;
        if (t < 0.0f)
        {
            t = t2;
            if (t >= 0.0f)
            {
                glm::vec3 localIntersectPos = _ray.origin + t * _ray.direction;
                float height = glm::dot(localIntersectPos - mPosition, axis);
                if (height >= 0.0f && height <= mHeight)
                {
                    tSide = t;
                }
            }
        }
        else 
        {
            glm::vec3 localIntersectPos = _ray.origin + t * _ray.direction;
            float height = glm::dot(localIntersectPos - mPosition, axis);
            if (height >= 0.0f && height <= mHeight)
            {
                tSide = t;
            }
        }
    }

    // Check for intersection with the top and bottom caps
    float tCapBottom = -1.0f;
    float tCapTop = -1.0f;
    glm::vec3 capCenter;
    glm::vec3 capNormal = axis;

    // Bottom cap
    capCenter = mPosition;
    float tCap = glm::dot(capCenter - _ray.origin, capNormal) / glm::dot(_ray.direction, capNormal);
    if (tCap >= 0.0f) {
        glm::vec3 intersectCap = _ray.origin + tCap * _ray.direction;
        if (glm::length(intersectCap - capCenter) <= mRadius)
        {
            tCapBottom = tCap;
        }
    }

    // Top cap
    capCenter = mPosition + axis * mHeight;
    tCap = glm::dot(capCenter - _ray.origin, capNormal) / glm::dot(_ray.direction, capNormal);
    if (tCap >= 0.0f)
    {
        glm::vec3 intersectCap = _ray.origin + tCap * _ray.direction;
        if (glm::length(intersectCap - capCenter) <= mRadius)
        {
            tCapTop = tCap;
        }
    }

    // Determine the closest intersection
    float tMin = std::numeric_limits<float>::max();
    bool intersected = false;

    if (tSide >= 0.0f && tSide < tMin)
    {
        tMin = tSide;
        intersected = true;
    }
    if (tCapBottom >= 0.0f && tCapBottom < tMin)
    {
        tMin = tCapBottom;
        intersected = true;
    }
    if (tCapTop >= 0.0f && tCapTop < tMin)
    {
        tMin = tCapTop;
        intersected = true;
    }

    if (intersected)
    {
        _intersectPosition = _ray.origin + tMin * _ray.direction;
        return true;
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