#include "blade.h"

using namespace std;

/* Define the vertex attributes for vertex positions and normals.
Make these match your application and vertex shader
You might also want to add colours and texture coordinates */
Blade::Blade()
{
	attribute_v_coord = 0;
	attribute_v_colours = 1;
	attribute_v_normal = 2;
	numvertices = 12;
	this->drawmode = drawmode;
}


Blade::~Blade()
{
}


/* Make a Blade from hard-coded vertex positions and normals  */
void Blade::makeBlade(glm::vec4 vertex_colour)
{
	/*
		A
		|\
		| \
		|  \ B
		|__/
		D  C

		Define the 8 veritices as vectors.
		Front side vertices: A, B, C, D
		Back side vertices: A', B', C', D' (A' = AA)
	*/

	// Front side
	glm::vec3 vertexA = glm::vec3(-0.05f, 0.6f, 0.01f);
	glm::vec3 vertexB = glm::vec3(0.1f, 0.15f, 0.01f);
	glm::vec3 vertexC = glm::vec3(0.05f, 0.f, 0.01f);
	glm::vec3 vertexD = glm::vec3(-0.05f, 0.f, 0.01f);
	
	// Back side
	glm::vec3 vertexAA = glm::vec3(-0.05f, 0.6f, -0.01f);
	glm::vec3 vertexBB = glm::vec3(0.1f, 0.15f, -0.01f);
	glm::vec3 vertexCC = glm::vec3(0.05f, 0.f, -0.01f);
	glm::vec3 vertexDD = glm::vec3(-0.05f, 0.f, -0.01f);

	/* Define vertices for a Blade in 12 triangles */
	GLfloat vertexPositions[] =
	{
		// Front face
		vertexA[0], vertexA[1], vertexA[2],
		vertexB[0], vertexB[1], vertexB[2],
		vertexD[0], vertexD[1], vertexD[2],

		vertexB[0], vertexB[1], vertexB[2],
		vertexC[0], vertexC[1], vertexC[2],
		vertexD[0], vertexD[1], vertexD[2],

		//Back face
		vertexAA[0], vertexAA[1], vertexAA[2],
		vertexBB[0], vertexBB[1], vertexBB[2],
		vertexDD[0], vertexDD[1], vertexDD[2],

		vertexBB[0], vertexBB[1], vertexBB[2],
		vertexCC[0], vertexCC[1], vertexCC[2],
		vertexDD[0], vertexDD[1], vertexDD[2],

		// Left face
		vertexAA[0], vertexAA[1], vertexAA[2],
		vertexA[0], vertexA[1], vertexA[2],
		vertexD[0], vertexD[1], vertexD[2],

		vertexAA[0], vertexAA[1], vertexAA[2],
		vertexD[0], vertexD[1], vertexD[2],
		vertexDD[0], vertexDD[1], vertexDD[2],

		// Right top face
		vertexA[0], vertexA[1], vertexA[2],
		vertexAA[0], vertexAA[1], vertexAA[2],
		vertexBB[0], vertexBB[1], vertexBB[2],

		vertexA[0], vertexA[1], vertexA[2],
		vertexBB[0], vertexBB[1], vertexBB[2],
		vertexB[0], vertexB[1], vertexB[2],

		// Right bottom face
		vertexB[0], vertexB[1], vertexB[2],
		vertexBB[0], vertexBB[1], vertexBB[2],
		vertexC[0], vertexC[1], vertexC[2],

		vertexBB[0], vertexBB[1], vertexBB[2],
		vertexCC[0], vertexCC[1], vertexCC[2],
		vertexC[0], vertexC[1], vertexC[2],

		// Bottom face
		vertexC[0], vertexC[1], vertexC[2],
		vertexCC[0], vertexCC[1], vertexCC[2],
		vertexDD[0], vertexDD[1], vertexDD[2],

		vertexC[0], vertexC[1], vertexC[2],
		vertexDD[0], vertexDD[1], vertexDD[2],
		vertexD[0], vertexD[1], vertexD[2],
	};

	/* Define colours for the Blade front parameter */
	glm::vec4 vertexColours[36];
	for (size_t i = 0; i < 36; i++)
	{
		vertexColours[i] = vertex_colour;
	}

	/* Calculate cross product for normals*/
	// ABxAB'
	glm::vec3 AB{ vertexB - vertexA };
	glm::vec3 ABB{ vertexBB - vertexA };
	glm::vec3 ABxABB = glm::cross(AB, ABB);

	// B'CxB'C'
	glm::vec3 BBC{ vertexC - vertexBB };
	glm::vec3 BBCC{ vertexCC - vertexBB };
	glm::vec3 BBCxBBCC = glm::cross(BBC, BBCC);

	/* Manually specified normals for the Blade */
	GLfloat normals[] =
	{
		// Front face
		0, 0, 1.f, 0, 0, 1.f, 0, 0, 1.f,
		0, 0, 1.f, 0, 0, 1.f, 0, 0, 1.f,

		//Back face
		0, 0, -1.f, 0, 0, -1.f, 0, 0, -1.f,
		0, 0, -1.f, 0, 0, -1.f, 0, 0, -1.f,

		// Left face
		-1.f, 0, 0, -1.f, 0, 0, -1.f, 0, 0,
		-1.f, 0, 0, -1.f, 0, 0, -1.f, 0, 0,

		// Right top face (ABxAB')
		ABxABB[0], ABxABB[1], ABxABB[2],
		ABxABB[0], ABxABB[1], ABxABB[2],
		ABxABB[0], ABxABB[1], ABxABB[2],

		ABxABB[0], ABxABB[1], ABxABB[2],
		ABxABB[0], ABxABB[1], ABxABB[2],
		ABxABB[0], ABxABB[1], ABxABB[2],

		// Right bottom face (B'CxB'C')
		BBCxBBCC[0], BBCxBBCC[1], BBCxBBCC[2],
		BBCxBBCC[0], BBCxBBCC[1], BBCxBBCC[2],
		BBCxBBCC[0], BBCxBBCC[1], BBCxBBCC[2],

		BBCxBBCC[0], BBCxBBCC[1], BBCxBBCC[2],
		BBCxBBCC[0], BBCxBBCC[1], BBCxBBCC[2],
		BBCxBBCC[0], BBCxBBCC[1], BBCxBBCC[2],

		// Bottom face
		0, -1.f, 0, 0, -1.f, 0, 0, -1.f, 0,
		0, -1.f, 0, 0, -1.f, 0, 0, -1.f, 0,
	};

	/* Create the vertex buffer for the Blade */
	glGenBuffers(1, &positionBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Create the colours buffer for the Blade */
	glGenBuffers(1, &colourObject);
	glBindBuffer(GL_ARRAY_BUFFER, colourObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexColours), vertexColours, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Create the normals  buffer for the Blade */
	glGenBuffers(1, &normalsBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, normalsBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


/* Draw the Blade by bining the VBOs and drawing triangles */
void Blade::drawBlade(int drawmode)
{
	/* Bind Blade vertices. Note that this is in attribute index attribute_v_coord */
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glEnableVertexAttribArray(attribute_v_coord);
	glVertexAttribPointer(attribute_v_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);

	/* Bind Blade colours. Note that this is in attribute index attribute_v_colours */
	glBindBuffer(GL_ARRAY_BUFFER, colourObject);
	glEnableVertexAttribArray(attribute_v_colours);
	glVertexAttribPointer(attribute_v_colours, 4, GL_FLOAT, GL_FALSE, 0, 0);

	/* Bind Blade normals. Note that this is in attribute index attribute_v_normal */
	glEnableVertexAttribArray(attribute_v_normal);
	glBindBuffer(GL_ARRAY_BUFFER, normalsBufferObject);
	glVertexAttribPointer(attribute_v_normal, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glPointSize(3.f);

	// Switch between filled and wireframe modes
	if (drawmode == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Draw points
	if (drawmode == 2)
	{
		glDrawArrays(GL_POINTS, 0, numvertices * 3);
	}
	else // Draw the Blade in triangles
	{
		glDrawArrays(GL_TRIANGLES, 0, numvertices * 3);
	}
}