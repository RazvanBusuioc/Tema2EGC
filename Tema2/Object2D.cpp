#include "Object2D.h"

#include <Core/Engine.h>
#include <iostream>


Mesh* Object2D::CreateSquare(std::string name, glm::vec3 leftBottomCorner, float length, float height,  glm::vec3 color, bool fill) {
	glm::vec3 corner = leftBottomCorner;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(length, 0, 0), color),
		VertexFormat(corner + glm::vec3(length, height, 0), color),
		VertexFormat(corner + glm::vec3(0, height, 0), color)
	};

	Mesh* square = new Mesh(name);
	std::vector<unsigned short> indices = { 0, 1, 2, 3 };

	if (!fill) {
		square->SetDrawMode(GL_LINE_LOOP);
	}
	else {
		indices.push_back(0);
		indices.push_back(2);
	}

	//square->InitFromData(vertices, indices);
	//return square;

	unsigned int VAO = 0;
	// TODO: Create the VAO and bind it
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// TODO: Create the VBO and bind it
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// TODO: Send vertices data into the VBO buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// TODO: Crete the IBO and bind it
	unsigned int IBO;
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	// TODO: Send indices data into the IBO buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// ========================================================================
	// This section describes how the GPU Shader Vertex Shader program receives data

	// set vertex position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

	glBindVertexArray(0);

	// Check for OpenGL errors
	CheckOpenGLError();

	// Mesh information is saved into a Mesh object
	Mesh* m = new Mesh(name);
	m->InitFromBuffer(VAO, static_cast<unsigned short>(indices.size()));
	m->vertices = vertices;
	m->indices = indices;
	return m;
}

Mesh* Object2D::CreateHeart(std::string name, glm::vec3 leftBottomCorner, float height, float length, glm::vec3 color, bool fill) {
	glm::vec3 corner = leftBottomCorner;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(0, height / 4, 0), color),
		VertexFormat(corner + glm::vec3(length / 4, height / 2, 0), color),
		VertexFormat(corner + glm::vec3(length / 2, height / 4, 0), color),
		VertexFormat(corner + glm::vec3(length / 2, 0, 0), color),
		VertexFormat(corner + glm::vec3(0, -height / 2, 0), color),
		VertexFormat(corner + glm::vec3(-length / 2, 0, 0), color),
		VertexFormat(corner + glm::vec3(-length / 2, height / 4, 0), color),
		VertexFormat(corner + glm::vec3(-length / 4, height / 2, 0), color)
	};

	Mesh* heart = new Mesh(name);
	if (!fill) {
		heart->SetDrawMode(GL_LINE_LOOP);
		std::vector<unsigned short> indices = { 1, 2, 3, 4, 5, 6, 7, 8, 1 };
		heart->InitFromData(vertices, indices);
	}
	else {
		std::vector<unsigned short> indices = { 0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 5, 0, 5, 6, 0, 6, 7, 0, 7, 8, 0, 8, 1 };
		heart->InitFromData(vertices, indices);
	}
	return heart;
}
