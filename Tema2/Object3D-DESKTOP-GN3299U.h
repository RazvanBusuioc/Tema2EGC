#pragma once

#include <string>

#include <include/glm.h>
#include <Core/GPU/Mesh.h>

namespace Object3D
{

	//set the vertex shader input variables
	Mesh* CreateMesh(std::string name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned short>& indices);

	//create the vertices and indices for a tile
	Mesh* CreateTile(std::string name, glm::vec3 dim, glm::vec3 color);

	Mesh* CreteTexturedTile(std::string name, glm::vec3 dimension);

	Mesh* CreateCylinder(std::string name, float height, float radius);

	std::vector<float> getUnitCircleVertices();

}


