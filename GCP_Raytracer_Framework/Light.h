#pragma once

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>

#include <string>
#include <GLM/glm.hpp>

struct Light
{
	Light(std::string _name, glm::vec3 _position, glm::vec3 _colour) : mName(_name), position(_position), colour(_colour) {}

	std::string mName = "Light";
	glm::vec3 position{ 0 };
	glm::vec3 colour{ 0 };

	void UpdateUI()
	{
		ImGui::Text(mName.c_str()); 
	
		ImGui::SliderFloat3(("Position " + mName).c_str(), &position[0], -100.0f, 100.0f);
		ImGui::ColorEdit3(("Colour " + mName).c_str(), &colour[0]);
	}
};