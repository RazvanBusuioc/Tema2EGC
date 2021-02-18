#include "Object3D.h"

#include <Core/Engine.h>
#include <iostream>


Mesh* Object3D::CreateMesh(std::string name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned short>& indices)
{
	unsigned int VAO = 0;
	//Create the VAO and bind it
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Create the VBO and bind it
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Send vertices data into the VBO buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// Crete the IBO and bind it
	unsigned int IBO;
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	// Send indices data into the IBO buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// ========================================================================
	// This section describes how the GPU Shader Vertex Shader program receives data

	// set vertex position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

	// set vertex normal attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

	// set texture coordinate attribute
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

	// set vertex color attribute
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));

	// ========================================================================

	// Unbind the VAO
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



Mesh* Object3D::CreateTile(std::string name, glm::vec3 dimension, glm::vec3 color)
{
	float dim_x = dimension.x/(float)2;
	float dim_y = dimension.y/(float)2;
	float dim_z = dimension.z/(float)2;

	glm::vec3 white_color = glm::vec3(0.8f, 0.8f, 0.8f);

	std::vector<VertexFormat> vertices
	{
		VertexFormat(glm::vec3(-dim_x, -dim_y,  dim_z), color),
		VertexFormat(glm::vec3(dim_x, -dim_y,  dim_z), color),
		VertexFormat(glm::vec3(-dim_x,  dim_y,  dim_z), color),
		VertexFormat(glm::vec3(dim_x,  dim_y,  dim_z), white_color),
		VertexFormat(glm::vec3(-dim_x, -dim_y, -dim_z), white_color),
		VertexFormat(glm::vec3(dim_x, -dim_y, -dim_z), color),
		VertexFormat(glm::vec3(-dim_x,  dim_y, -dim_z), color),
		VertexFormat(glm::vec3(dim_x,  dim_y, -dim_z), color)
	};

	std::vector<unsigned short> indices =
	{
		0, 1, 2,		1, 3, 2,
		2, 3, 7,		2, 7, 6,
		1, 7, 3,		1, 5, 7,
		6, 7, 4,		7, 5, 4,
		0, 4, 1,		1, 4, 5,
		2, 6, 4,		0, 2, 4,
	};

	return CreateMesh(name, vertices, indices);
}
