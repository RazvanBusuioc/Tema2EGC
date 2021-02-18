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

Mesh* Object3D::CreteTexturedTile(std::string name, glm::vec3 dimension) {
	float dim_x = dimension.x / (float)2;
	float dim_y = dimension.y / (float)2;
	float dim_z = dimension.z / (float)2;

	glm::vec3 white_color = glm::vec3(0.8f, 0.8f, 0.8f);

	std::vector<glm::vec3> vertices
	{
		//0
		glm::vec3(-dim_x, -dim_y,  dim_z), //0 ------1
		glm::vec3(-dim_x, -dim_y,  dim_z), //1---5
		glm::vec3(-dim_x, -dim_y,  dim_z), //2--6
		//1
		glm::vec3(dim_x, -dim_y,  dim_z), //3---1
		glm::vec3(dim_x, -dim_y,  dim_z), //4---3
		glm::vec3(dim_x, -dim_y,  dim_z), //5---5
		//2
		glm::vec3(-dim_x,  dim_y,  dim_z), //6----1
		glm::vec3(-dim_x,  dim_y,  dim_z), //7---2
		glm::vec3(-dim_x,  dim_y,  dim_z), //8--6
		//3
		glm::vec3(dim_x,  dim_y,  dim_z), //9---1
		glm::vec3(dim_x,  dim_y,  dim_z), //10---2
		glm::vec3(dim_x,  dim_y,  dim_z), //11---3
		//4
		glm::vec3(-dim_x, -dim_y, -dim_z), //12--4
		glm::vec3(-dim_x, -dim_y, -dim_z), //13---5
		glm::vec3(-dim_x, -dim_y, -dim_z), //14--6
		//5
		glm::vec3(dim_x, -dim_y, -dim_z), //15---3
		glm::vec3(dim_x, -dim_y, -dim_z), //16--4
		glm::vec3(dim_x, -dim_y, -dim_z), //17---5
		//6
		glm::vec3(-dim_x,  dim_y, -dim_z), //18---2
		glm::vec3(-dim_x,  dim_y, -dim_z), //19--4
		glm::vec3(-dim_x,  dim_y, -dim_z), //20--6
		//7
		glm::vec3(dim_x,  dim_y, -dim_z), //21---2
		glm::vec3(dim_x,  dim_y, -dim_z), //22--3
		glm::vec3(dim_x,  dim_y, -dim_z)  //23--4
	};

	std::vector<glm::vec2> texCoords{
		glm::vec2(1, 0), //0 --1
		glm::vec2(1, 0), //1 --5
		glm::vec2(0, 0), //2 --6

		glm::vec2(0, 0), //3 --1
		glm::vec2(1, 0), //4 --3
		glm::vec2(1, 1), //5 --5

		glm::vec2(1, 1), //6 --1
		glm::vec2(0, 1), //7 --2
		glm::vec2(0, 1), //8 --6

		glm::vec2(0, 1), //9 --1
		glm::vec2(1, 1), //10 --2
		glm::vec2(1, 1), //11 --3

		glm::vec2(0, 0), //12 --4
		glm::vec2(0, 0), //13 --5
		glm::vec2(1, 0), //14 --6

		glm::vec2(0, 0), //15 --3
		glm::vec2(1, 0), //16 --4
		glm::vec2(0, 1), //17 --5

		glm::vec2(0, 0), //18 --2
		glm::vec2(0, 1), //19 --4
		glm::vec2(1, 1), //20 --6

		glm::vec2(1, 0), //21 --2
		glm::vec2(0, 1), //22 --3
		glm::vec2(1, 1), //23 --4
	};

	std::vector<glm::vec3> normals{
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),
		glm::vec3(0,1,0)
	};

	/*std::vector<unsigned short> indices =
	{
		0, 1, 2,		1, 3, 2,
		2, 3, 7,		2, 7, 6,
		1, 7, 3,		1, 5, 7,
		6, 7, 4,		7, 5, 4,
		0, 4, 1,		1, 4, 5,
		2, 6, 4,		0, 2, 4,
	};*/

	std::vector<unsigned short> indices =
	{
		0, 3, 6,		3, 9, 6,
		7, 10, 21,		7, 21, 18,
		4, 22, 11,		4, 15, 22,
		19, 23, 12,		23, 16, 12,
		1, 13, 5,		5, 13, 17,
		8, 20, 14,		2, 8, 14,
	};

	Mesh* mesh = new Mesh("name");
	mesh->InitFromData(vertices, normals, texCoords, indices);
	return mesh;
}

Mesh* Object3D::CreateCylinder(std::string name, float height, float radius)
{
	float sectorCount = 16;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;
	std::vector<unsigned short> indices;


	std::vector<float> unitVertices = getUnitCircleVertices();
	for (int i = 0; i < 2; ++i)
	{
		float h = -height / 2.0f + i * height;           // z value; -h/2 to h/2
		float t = 1.0f - i;                              // vertical tex coord; 1 to 0

		for (int j = 0, k = 0; j <= sectorCount; ++j, k += 3)
		{
			float ux = unitVertices[k];
			float uy = unitVertices[k + 1];
			float uz = unitVertices[k + 2];

			// position vector
			vertices.push_back(glm::vec3((ux * radius),(uy * radius), h));

			// normal vector
			normals.push_back(glm::vec3(ux, uy, uz)); 

			// texture coordinate
			texCoords.push_back(glm::vec2((float)j / sectorCount, t));
		}
	}
	std::cout << "len is " << vertices.size() << std::endl;


	for (int j = 0; j <= sectorCount; j++) {
		indices.push_back(j); indices.push_back(j + 1); indices.push_back(j + sectorCount + 1);
		indices.push_back(j + 1); indices.push_back(j + sectorCount + 2); indices.push_back(j + sectorCount + 1);
	}

	int baseCenterIndex = (int)vertices.size();
	int topCenterIndex = baseCenterIndex + sectorCount + 1; // include center vertex

	for (int i = 0; i < 2; ++i)
	{
		float h = -height / 2.0f + i * height;           // z value; -h/2 to h/2
		float nz = -1 + i * 2;                           // z value of normal; -1 to 1

		// center point
		vertices.push_back(glm::vec3(0, 0, h));
		normals.push_back(glm::uvec3(0, 0, nz));
		texCoords.push_back(glm::vec2(0.5f, 0.5f));

		for (int j = 0, k = 0; j < sectorCount; ++j, k += 3)
		{
			float ux = unitVertices[k];
			float uy = unitVertices[k + 1];
			// position vector
			vertices.push_back(glm::vec3((ux * radius), (uy * radius), h));
			// normal vector
			normals.push_back(glm::vec3(0, 0, nz));
			// texture coordinate
			texCoords.push_back(glm::vec2(-ux * 0.5f + 0.5f, -uy * 0.5f + 0.5f));
		}
	}

	for (int i = 0, k = baseCenterIndex + 1; i < sectorCount; ++i, ++k)
	{
		if (i < sectorCount - 1)
		{
			indices.push_back(baseCenterIndex);
			indices.push_back(k + 1);
			indices.push_back(k);
		}
		else // last triangle
		{
			indices.push_back(baseCenterIndex);
			indices.push_back(baseCenterIndex + 1);
			indices.push_back(k);
		}
	}

	// indices for the top surface
	for (int i = 0, k = topCenterIndex + 1; i < sectorCount; ++i, ++k)
	{
		if (i < sectorCount - 1)
		{
			indices.push_back(topCenterIndex);
			indices.push_back(k);
			indices.push_back(k + 1);
		}
		else // last triangle
		{
			indices.push_back(topCenterIndex);
			indices.push_back(k);
			indices.push_back(topCenterIndex + 1);
		}
	}


	Mesh* mesh = new Mesh("crate");
	mesh->InitFromData(vertices, normals, texCoords, indices);
	return mesh;
}


std::vector<float> Object3D::getUnitCircleVertices()
{
	float sectorCount = 16;

	const float PI = 3.1415926f;
	float sectorStep = 2 * PI / sectorCount;
	float sectorAngle;  // radian

	std::vector<float> unitCircleVertices;
	for (int i = 0; i <= sectorCount; ++i)
	{
		sectorAngle = i * sectorStep;
		unitCircleVertices.push_back(cos(sectorAngle)); // x
		unitCircleVertices.push_back(sin(sectorAngle)); // y
		unitCircleVertices.push_back(0);                // z
	}
	return unitCircleVertices;
}