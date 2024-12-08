#include "RayObject.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <iostream>

glm::vec3 RayObject::ShadeAtPosition(glm::vec3 _intersectPosition, Light *_light, glm::vec3 _camPos, bool _pbr)
{
	if (mIsLight)
		return mAlbedo;

    if (_pbr)
        return CalculatePBR(_intersectPosition, _light, _camPos);
    else
        return CalulateDiffuseAndSpecular(_intersectPosition, _light, _camPos);
	
}

glm::vec3 RayObject::CalulateDiffuseAndSpecular(glm::vec3 _intersectPosition, Light* _light, glm::vec3 _camPos)
{
    glm::vec3 lightDir = glm::normalize(_light->GetPosition() - _intersectPosition);

	glm::vec3 normal = NormalAtPosition(_intersectPosition);
	glm::vec3 eyeDir = glm::normalize(_camPos - _intersectPosition);

	glm::vec3 diffuse = (glm::max(glm::dot(normal, lightDir), 0.f)) * mAlbedo * _light->GetColour();

	glm::vec3 reflectDir = glm::reflect(-lightDir, normal);
	float spec = glm::pow(glm::max(glm::dot(eyeDir, reflectDir), 0.f), 32);
	glm::vec3 specular = spec * mShininess * _light->GetColour();

	return diffuse + specular;
}

glm::vec3 RayObject::CalculatePBR(glm::vec3 _intersectPosition, Light* _light, glm::vec3 _camPos)
{
    glm::vec3 normal = NormalAtPosition(_intersectPosition);
    glm::vec3 eyeDir = glm::normalize(_camPos - _intersectPosition);

    glm::vec3 lightDir = glm::normalize(_light->GetPosition() - _intersectPosition);

    glm::vec3 halfVector = glm::normalize(lightDir + eyeDir);

    float NdotL = glm::max(glm::dot(normal, lightDir), 0.0f);
    float NdotH = glm::max(glm::dot(normal, halfVector), 0.0f);
    float VdotH = glm::max(glm::dot(eyeDir, halfVector), 0.0f);

    float roughness = mRoughness;
    float F0 = 0.04f;
    glm::vec3 F = glm::vec3(F0) + (mAlbedo - glm::vec3(F0)) * glm::pow(1.0f - VdotH, 5.0f);

    float NDF = (roughness + 1) * glm::pow(NdotH, roughness) / (2.0f * 3.1415 * glm::pow(1.0f + (roughness * roughness - 1.0f) * NdotH * NdotH, 2.0f));
    float G = glm::min(1.0f, glm::min(2.0f * NdotH * NdotL / VdotH, 2.0f * NdotH * glm::dot(normal, eyeDir) / VdotH));

    glm::vec3 kS = F;
    glm::vec3 kD = glm::vec3(1.0f) - kS;
    kD *= 1.0f - mMetallic;

    glm::vec3 numerator = NDF * G * F;
    float denominator = 4.0f * glm::max(4.0f * NdotL * NdotH, 0.001f);
    glm::vec3 specular = numerator / denominator;

    glm::vec3 ambient = glm::vec3(0.00f) * mAlbedo * _light->GetColour();

    glm::vec3 colour = (kD * mAlbedo / 3.1415f) + specular;

    return (colour * NdotL + ambient) * _light->GetColour();
}

glm::vec3 RayObject::fresnelSchlick(float cosTheta, const glm::vec3& F0) {
    return F0 + (1.0f - F0) * glm::pow(glm::clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f);
}

float RayObject::DistributionGGX(const glm::vec3& N, const glm::vec3& H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = glm::max(glm::dot(N, H), 0.0f);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = 3.1415 * denom * denom;

    return num / denom;
}

float RayObject::GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;

    float num = NdotV;
    float denom = NdotV * (1.0f - k) + k;

    return num / denom;
}

float RayObject::GeometrySmith(const glm::vec3& N, const glm::vec3& V, const glm::vec3& L, float roughness) {
    float NdotV = glm::max(glm::dot(N, V), 0.0f);
    float NdotL = glm::max(glm::dot(N, L), 0.0f);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

void RayObject::UpdateUI()
{
    if (mIsLight)
		return;

    ImGui::Text(mName.c_str());

    ImGui::SliderFloat(("Shininess(pbr) " + mName).c_str(), &mShininess, 0.0f, 1.0f);

    ImGui::ColorEdit3(("Albedo" + mName).c_str(), &mAlbedo[0]);

    ImGui::SliderFloat(("Metallic" + mName).c_str(), &mMetallic, 0.0f, 1.0f);

    ImGui::SliderFloat(("Roughness" + mName).c_str(), &mRoughness, 0.0f, 1.0f);

    ImGui::SliderFloat(("Reflectivity" + mName).c_str(), &mReflectivity, 0.0f, 1.0f);

    ImGui::SliderFloat(("Refractive Index" + mName).c_str(), &mRefractiveIndex, 1.0f, 2.0f);

    ImGui::SliderFloat(("Transparency" + mName).c_str(), &mTransparency, 0.0f, 1.0f);

    ImGui::SliderFloat3(("Position" + mName).c_str(), &mPosition[0], -100, 100);
}