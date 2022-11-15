/* sphere.h
 Example class to create a generic sphere object
 Resolution can be controlled by settign nuber of latitudes and longitudes
 Iain Martin October 2019
*/

#pragma once

#include "wrapper_glfw.h"
#include <vector>
#include <glm/glm.hpp>

class Sphere
{
public:
	Sphere();
	~Sphere();

	void makeSphere(GLuint numlats, GLuint numlongs, glm::vec3 RGB=glm::vec3(1.f, 1.f, 1.f));
	void drawSphere(int drawmode);

	// Define vertex buffer object names (e.g as globals)
	GLuint sphereBufferObject;
	GLuint sphereNormals;
	GLuint sphereColours;
	GLuint elementbuffer;

	GLuint attribute_v_coord;
	GLuint attribute_v_normal;
	GLuint attribute_v_colours;

	GLuint numspherevertices;
	GLuint numlats;
	GLuint numlongs;
	int drawmode;

private:
	void makeUnitSphere(GLfloat *pVertices);
};
