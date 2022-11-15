/* Link to static libraries, could define these as linker inputs in the project settings instead
if you prefer */
#ifdef _DEBUG
#pragma comment(lib, "glfw3D.lib")
#else
#pragma comment(lib, "glfw3.lib")
#endif
#pragma comment(lib, "opengl32.lib")

/* Include the header to the GLFW wrapper class which
   also includes the OpenGL extension initialisation*/
#include "wrapper_glfw.h"
#include <iostream>
#include <stack>

   /* Include GLM core and matrix extensions*/
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

// Include headers for our objects
#include "objects/sphere.h"
#include "objects/cube.h"

using namespace std;
using namespace glm;

const int NUM_PROGRAMS = 3;

GLuint program[NUM_PROGRAMS];		/* Identifiers for the shader prgorams */
GLuint current_program;
GLuint vao;			/* Vertex array (Containor) object. This is the index of the VAO that will be the container for
					   our buffer objects */

GLuint colourmode;	/* Index of a uniform to switch the colour mode in the vertex shader
					  I've included this to show you how to pass in an unsigned integer into
					  your vertex shader. */
GLuint emitmode;
GLuint attenuationmode;

/* Position and view globals */
GLfloat angle_x, angle_inc_x, x, model_scale, z, y, vx, vy, vz;
GLfloat angle_y, angle_inc_y, angle_z, angle_inc_z;
GLuint drawmode;			// Defines drawing mode of sphere as points, lines or filled polygons
GLuint numlats, numlongs;	//Define the resolution of the sphere object

GLfloat light_x, light_y, light_z;

/* Uniforms*/
GLuint modelID[NUM_PROGRAMS], viewID[NUM_PROGRAMS], projectionID[NUM_PROGRAMS], lightposID[NUM_PROGRAMS], normalmatrixID[NUM_PROGRAMS];
GLuint colourmodeID[NUM_PROGRAMS], emitmodeID[NUM_PROGRAMS], attenuationmodeID[NUM_PROGRAMS];

GLfloat aspect_ratio;		/* Aspect ratio of the window defined in the reshape callback*/
GLuint numspherevertices;

/* Primitive objects */
Cube baseCube;
Cube bodyCube;
Cube headCube;
Sphere rotorSphere;
Cube blade1Cube;
Cube blade2Cube;

/* Position of primitives */
GLfloat base_x, base_y, base_z;
GLfloat body_x, body_y, body_z;
GLfloat head_x, head_y, head_z;
GLfloat rotor_x, rotor_y, rotor_z;
GLfloat blade1_x, blade1_y, blade1_z;
GLfloat blade2_x, blade2_y, blade2_z;

/* Scale of primitives */
GLfloat base_scale_x, base_scale_y, base_scale_z;
GLfloat body_scale_x, body_scale_y, body_scale_z;
GLfloat head_scale_x, head_scale_y, head_scale_z;
GLfloat rotor_scale_x, rotor_scale_y, rotor_scale_z;
GLfloat blade_scale_x, blade_scale_y, blade_scale_z;

/* Rotation of primitives */
GLfloat rotation_z;
GLfloat rotation_inc_z;

/*
This function is called before entering the main rendering loop.
Use it for all your initialisation stuff
*/
void init(GLWrapper* glw)
{
	/* Set the object transformation controls to their initial values */
	x = 0.05f;
	y = 0;
	z = 0;
	vx = 0; vx = 0, vz = 4.f;
	light_x = 0; light_y = 0; light_z = 0;
	angle_x = angle_y = angle_z = 0;
	angle_inc_x = angle_inc_y = angle_inc_z = 0; const float roughness = 0.8;
	model_scale = 1.f;
	aspect_ratio = 1.3333f;
	colourmode = 0;
	emitmode = 0;
	attenuationmode = 1; // Attenuation is on by default
	numlats = 40;		// Number of latitudes in our sphere
	numlongs = 40;		// Number of longitudes in our sphere

	base_x = 0; base_y = 0; base_z = 0;
	body_x = 0; body_y = 0.5f; body_z = 0;
	head_x = 0; head_y = 1.f; head_z = 0;
	rotor_x = 0; rotor_y = 1.f; rotor_z = 0.25f;
	blade1_x = 0; blade1_y = 1.f; blade1_z = 0.3f;
	blade2_x = 0; blade2_y = 1.f; blade2_z = 0.3;
	
	base_scale_x = 1.f; base_scale_y = 0.1f; base_scale_z = 1.f;
	body_scale_x = 0.5f; body_scale_y = 2.f; body_scale_z = 0.5f;
	head_scale_x = 0.6f; head_scale_y = 0.5f; head_scale_z = 0.9f;
	rotor_scale_x = 0.12f; rotor_scale_y = 0.12f; rotor_scale_z = 0.2f;
	blade_scale_x = 0.15f; blade_scale_y = 2.f; blade_scale_z = 0.05f;

	rotation_z = 0;
	rotation_inc_z = 0;

	// Generate index (name) for one vertex array object
	glGenVertexArrays(1, &vao);

	// Create the vertex array object and make it current
	glBindVertexArray(vao);

	/* Load and build the vertex and fragment shaders */
	try
	{
		program[0] = glw->LoadShader("shaders\\poslight.vert", "shaders\\poslight.frag");
		program[1] = glw->LoadShader("shaders\\fraglight.vert", "shaders\\fraglight.frag");
		program[2] = glw->LoadShader("shaders\\fraglight.vert", "shaders\\fraglight_oren_nayar.frag");
	}
	catch (exception& e)
	{
		cout << "Caught exception: " << e.what() << endl;
		cin.ignore();
		exit(0);
	}

	/* Define the same uniforms to send to both shaders */
	for (int i = 0; i < NUM_PROGRAMS; i++)
	{
		glUseProgram(program[i]);
		modelID[i] = glGetUniformLocation(program[i], "model");
		colourmodeID[i] = glGetUniformLocation(program[i], "colourmode");
		emitmodeID[i] = glGetUniformLocation(program[i], "emitmode");
		attenuationmodeID[i] = glGetUniformLocation(program[i], "attenuationmode");
		viewID[i] = glGetUniformLocation(program[i], "view");
		projectionID[i] = glGetUniformLocation(program[i], "projection");
		lightposID[i] = glGetUniformLocation(program[i], "lightpos");
		normalmatrixID[i] = glGetUniformLocation(program[i], "normalmatrix");
	}
	// Define the index which represents the current shader (i.e. default is gouraud)
	current_program = 0;

	/* create our sphere and cube objects */
	baseCube.makeCube();
	bodyCube.makeCube();
	headCube.makeCube();
	rotorSphere.makeSphere(numlats, numlongs);
	blade1Cube.makeCube();
}

/* Called to update the display. Note that this function is called in the event loop in the wrapper
   class because we registered display as a callback function */
void display()
{
	/* Define the background colour */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	/* Clear the colour and frame buffers */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Enable depth test  */
	glEnable(GL_DEPTH_TEST);

	/* Make the compiled shader program current */
	glUseProgram(program[current_program]);


	// Define our model transformation in a stack and 
	// push the identity matrix onto the stack
	stack<mat4> model;
	model.push(mat4(1.0f));

	// Define the normal matrix
	mat3 normalmatrix;

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	mat4 projection = perspective(radians(30.0f), aspect_ratio, 0.1f, 100.0f);

	// Camera matrix
	mat4 view = lookAt(
		vec3(0, 3, 5), // Camera is at (0,0,4), in World Space
		vec3(0, 0, 0), // and looks at the origin
		vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	// Apply rotations to the view position. This wil get applied to the whole scene
	view = rotate(view, -radians(vx), vec3(1, 0, 0)); //rotating in clockwise direction around x-axis
	view = rotate(view, -radians(vy), vec3(0, 1, 0)); //rotating in clockwise direction around y-axis
	view = rotate(view, -radians(vz), vec3(0, 0, 1));

	// Define the light position and transform by the view matrix
	vec4 lightpos = view * vec4(light_x, light_y, light_z, 1.0);

	// Send our projection and view uniforms to the currently bound shader
	// I do that here because they are the same for all objects
	glUniform1ui(colourmodeID[current_program], colourmode);
	glUniformMatrix4fv(viewID[current_program], 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projectionID[current_program], 1, GL_FALSE, &projection[0][0]);
	glUniform4fv(lightposID[current_program], 1, &lightpos[0]);
	glUniform1ui(attenuationmodeID[current_program], attenuationmode);

	/* Draw a small sphere in the lightsource position to visually represent the light source */
	model.push(model.top());
	{
		model.top() = translate(model.top(), vec3(light_x, light_y, light_z));
		model.top() = scale(model.top(), vec3(0.05f, 0.05f, 0.05f)); // make a small sphere
		// Recalculate the normal matrix and send the model and normal matrices to the vertex shader																							// Recalculate the normal matrix and send to the vertex shader																								// Recalculate the normal matrix and send to the vertex shader																								// Recalculate the normal matrix and send to the vertex shader																						// Recalculate the normal matrix and send to the vertex shader
		glUniformMatrix4fv(modelID[current_program], 1, GL_FALSE, &(model.top()[0][0]));
		normalmatrix = transpose(inverse(mat3(view * model.top())));
		glUniformMatrix3fv(normalmatrixID[current_program], 1, GL_FALSE, &normalmatrix[0][0]);

		/* Draw our lightposition sphere  with emit mode on*/
		emitmode = 1;
		glUniform1ui(emitmodeID[current_program], emitmode);
		rotorSphere.drawSphere(drawmode);
		emitmode = 0;
		glUniform1ui(emitmodeID[current_program], emitmode);
	}
	model.pop();

	// Define the global model transformations (rotate and scale). Note, we're not modifying the light source position
	//model.top() = scale(model.top(), vec3(model_scale, model_scale, model_scale));
	model.top() = rotate(model.top(), -radians(angle_x), glm::vec3(1, 0, 0)); //rotating in clockwise direction around x-axis
	model.top() = rotate(model.top(), -radians(angle_y), glm::vec3(0, 1, 0)); //rotating in clockwise direction around y-axis
	model.top() = rotate(model.top(), -radians(angle_z), glm::vec3(0, 0, 1)); //rotating in clockwise direction around z-axis

	// This block of code draws the base cube
	model.push(model.top());
	{
		// Define the model transformations for the cube
		model.top() = translate(model.top(), vec3(base_x, base_y, base_z));
		model.top() = scale(model.top(), vec3(model_scale, base_scale_y, model_scale));

		// Send the model uniform and normal matrix to the currently bound shader,
		glUniformMatrix4fv(modelID[current_program], 1, GL_FALSE, &(model.top()[0][0]));

		// Recalculate the normal matrix and send to the vertex shader
		normalmatrix = transpose(inverse(mat3(view * model.top())));
		glUniformMatrix3fv(normalmatrixID[current_program], 1, GL_FALSE, &normalmatrix[0][0]);

		/* Draw our cube*/
		baseCube.drawCube(drawmode);
	}
	model.pop();

	// This block of code draws the body cube
	model.push(model.top());
	{
		// Define the model transformations for the cube
		model.top() = translate(model.top(), vec3(body_x, body_y, body_z));
		model.top() = scale(model.top(), vec3(body_scale_x, body_scale_y, body_scale_z));

		// Send the model uniform and normal matrix to the currently bound shader,
		glUniformMatrix4fv(modelID[current_program], 1, GL_FALSE, &(model.top()[0][0]));

		// Recalculate the normal matrix and send to the vertex shader
		normalmatrix = transpose(inverse(mat3(view * model.top())));
		glUniformMatrix3fv(normalmatrixID[current_program], 1, GL_FALSE, &normalmatrix[0][0]);

		/* Draw our cube*/
		bodyCube.drawCube(drawmode);
	}
	model.pop();

	// This block of code draws the head cube
	model.push(model.top());
	{
		// Define the model transformations for the cube
		model.top() = translate(model.top(), vec3(head_x, head_y, head_z));
		model.top() = scale(model.top(), vec3(head_scale_x, head_scale_y, head_scale_z));

		// Send the model uniform and normal matrix to the currently bound shader,
		glUniformMatrix4fv(modelID[current_program], 1, GL_FALSE, &(model.top()[0][0]));

		// Recalculate the normal matrix and send to the vertex shader
		normalmatrix = transpose(inverse(mat3(view * model.top())));
		glUniformMatrix3fv(normalmatrixID[current_program], 1, GL_FALSE, &normalmatrix[0][0]);

		/* Draw our cube*/
		headCube.drawCube(drawmode);
	}
	model.pop();


	// This block of code draws the rotor sphere
	model.push(model.top());
	{
		model.top() = translate(model.top(), vec3(rotor_x, rotor_y, rotor_z));
		model.top() = rotate(model.top(), radians(rotation_z), glm::vec3(0, 0, 1));
		model.top() = scale(model.top(), vec3(rotor_scale_x, rotor_scale_y, rotor_scale_z));

		// Recalculate the normal matrix and send the model and normal matrices to the vertex shader																							// Recalculate the normal matrix and send to the vertex shader																								// Recalculate the normal matrix and send to the vertex shader																								// Recalculate the normal matrix and send to the vertex shader																						// Recalculate the normal matrix and send to the vertex shader
		glUniformMatrix4fv(modelID[current_program], 1, GL_FALSE, &(model.top()[0][0]));
		normalmatrix = transpose(inverse(mat3(view * model.top())));
		glUniformMatrix3fv(normalmatrixID[current_program], 1, GL_FALSE, &normalmatrix[0][0]);

		rotorSphere.drawSphere(drawmode);
	}
	model.pop();

	// This block of code draws the blade1 cube
	model.push(model.top());
	{
		// Define the model transformations for the cube
		model.top() = translate(model.top(), vec3(blade1_x, blade1_y, blade1_z));
		model.top() = rotate(model.top(), radians(rotation_z), glm::vec3(0, 0, 1));
		model.top() = scale(model.top(), vec3(blade_scale_x, blade_scale_y, blade_scale_z));

		// Send the model uniform and normal matrix to the currently bound shader,
		glUniformMatrix4fv(modelID[current_program], 1, GL_FALSE, &(model.top()[0][0]));

		// Recalculate the normal matrix and send to the vertex shader
		normalmatrix = transpose(inverse(mat3(view * model.top())));
		glUniformMatrix3fv(normalmatrixID[current_program], 1, GL_FALSE, &normalmatrix[0][0]);

		/* Draw our cube*/
		blade1Cube.drawCube(drawmode);
	}
	model.pop();

	// This block of code draws the blade2 cube
	model.push(model.top());
	{
		// Define the model transformations for the cube
		model.top() = translate(model.top(), vec3(blade2_x, blade2_y, blade2_z));
		model.top() = rotate(model.top(), radians(90.f + rotation_z), glm::vec3(0, 0, 1));
		model.top() = scale(model.top(), vec3(blade_scale_x, blade_scale_y, blade_scale_z));

		// Send the model uniform and normal matrix to the currently bound shader,
		glUniformMatrix4fv(modelID[current_program], 1, GL_FALSE, &(model.top()[0][0]));

		// Recalculate the normal matrix and send to the vertex shader
		normalmatrix = transpose(inverse(mat3(view * model.top())));
		glUniformMatrix3fv(normalmatrixID[current_program], 1, GL_FALSE, &normalmatrix[0][0]);

		/* Draw our cube*/
		blade1Cube.drawCube(drawmode);
	}
	model.pop();


	glDisableVertexAttribArray(0);
	glUseProgram(0);

	/* Modify our animation variables */
	angle_x += angle_inc_x;
	angle_y += angle_inc_y;
	angle_z += angle_inc_z;
	rotation_z += rotation_inc_z;
}

/* Called whenever the window is resized. The new window size is given, in pixels. */
static void reshape(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	aspect_ratio = ((float)w / 640.f * 4.f) / ((float)h / 480.f * 3.f);
}

/* change view angle, exit upon ESC */
static void keyCallback(GLFWwindow* window, int key, int s, int action, int mods)
{
	/* Enable this call if you want to disable key responses to a held down key*/
//	if (action != GLFW_PRESS) return;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == 'Q') angle_inc_x -= 0.05f;
	if (key == 'W') angle_inc_x += 0.05f;
	if (key == 'E') angle_inc_y -= 0.05f;
	if (key == 'R') angle_inc_y += 0.05f;
	if (key == 'T') angle_inc_z -= 0.05f;
	if (key == 'Y') angle_inc_z += 0.05f;
	if (key == 'A') model_scale -= 0.02f;
	if (key == 'S') model_scale += 0.02f;
	if (key == 'Z') x -= 0.05f;
	if (key == 'X') x += 0.05f;
	if (key == 'C') y -= 0.05f;
	if (key == 'V') y += 0.05f;
	if (key == 'B') z -= 0.05f;
	if (key == 'N') z += 0.05f;
	if (key == '1') light_x -= 0.05f;
	if (key == '2') light_x += 0.05f;
	if (key == '3') light_y -= 0.05f;
	if (key == '4') light_y += 0.05f;
	if (key == '5') light_z -= 0.05f;
	if (key == '6') light_z += 0.05f;
	if (key == '7') vx -= 1.f;
	if (key == '8') vx += 1.f;
	if (key == '9') vy -= 1.f;
	if (key == '0') vy += 1.f;
	if (key == 'O') vz -= 1.f;
	if (key == 'P') vz += 1.f;

	/* Control rotor and blade rotation */
	if (key == 'L') rotation_inc_z -= 0.1f;
	if (key == 'K') rotation_inc_z += 0.1f;

	/* Switch colour mode */
	if (key == 'M' && action != GLFW_PRESS)
	{
		colourmode = !colourmode;
	}

	/* Turn attenuation on and off */
	if (key == '.' && action != GLFW_PRESS)
	{
		attenuationmode = !attenuationmode;
	}

	/* Cycle between drawing vertices, mesh and filled polygons */
	if (key == ',' && action != GLFW_PRESS)
	{
		drawmode++;
		if (drawmode > 2) drawmode = 0;
	}

	/* Switch between the vertex lighting shaders and the fragment shader */
	if (key == 'I' && action != GLFW_PRESS)
	{
		current_program++;
		if (current_program == 3) current_program = 0;
	}
}



/* Entry point of program */
int main(int argc, char* argv[])
{
	GLWrapper* glw = new GLWrapper(1024, 768, "Assessment 1");;

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD. Exiting." << endl;
		return -1;
	}

	glw->setRenderer(display);
	glw->setKeyCallback(keyCallback);
	glw->setReshapeCallback(reshape);

	init(glw);

	glw->eventLoop();

	delete(glw);
	return 0;
}