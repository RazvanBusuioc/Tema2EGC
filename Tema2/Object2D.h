#pragma once

#include <string>

#include <include/glm.h>
#include <Core/GPU/Mesh.h>

#define BOW_WIDTH 3

namespace Object2D
{
	Mesh* CreateHeart(std::string name, glm::vec3 leftBottomCorner, float height, float length, glm::vec3 color, bool fill = true);

	Mesh* CreateSquare(std::string name, glm::vec3 leftBottomCorner, float length, float height, glm::vec3 color, bool fill = true);
}


