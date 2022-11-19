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
#include "objects/cylinder.h"
#include "objects/blade.h"

using namespace std;
using namespace glm;

/* Identifiers for the shader programs */
GLuint program_phong;
GLuint program_oren_nayar;

GLuint vao;			/* Vertex array (Containor) object. This is the index of the VAO that will be the container for
					   our buffer objects */

GLuint colourmode;	/* Index of a uniform to switch the colour mode in the vertex shader
					  I've included this to show you how to pass in an unsigned integer into
					  your vertex shader. */
GLuint emitmode;
GLuint attenuationmode;

/* Position and view globals */
GLfloat angle_x, angle_inc_x, model_scale, vx, vy, vz;
GLfloat angle_y, angle_inc_y, angle_z, angle_inc_z;
GLuint drawmode;			// Defines drawing mode of sphere as points, lines or filled polygons
GLuint numlats, numlongs;	//Define the resolution of the sphere object

GLfloat light_x, light_y, light_z;

// Camera coordinates (x,y,z)
GLfloat camera_x, camera_y, camera_z;

/* Uniforms*/
GLuint phong_modelID, phong_viewID, phong_projectionID, phong_lightposID, phong_normalmatrixID;
GLuint phong_colourmodeID, phong_emitmodeID, phong_attenuationmodeID;
/* Uniforms*/
GLuint oren_nayar_modelID, oren_nayar_viewID, oren_nayar_projectionID, oren_nayar_lightposID, oren_nayar_normalmatrixID;
GLuint oren_nayar_colourmodeID, oren_nayar_emitmodeID, oren_nayar_attenuationmodeID;

GLfloat aspect_ratio;		/* Aspect ratio of the window defined in the reshape callback*/
GLuint numspherevertices;

/* Primitive objects */
Cube baseCube;
Cylinder tower;
Cube headCube;
Sphere rotorSphere;
Blade blade;

/* Position of primitives */
GLfloat origin_x, origin_y, origin_z;
GLfloat base_x, base_y, base_z;
GLfloat tower_x, tower_y, tower_z;
GLfloat body_x, body_y, body_z;
GLfloat rotor_x, rotor_y, rotor_z;

/* Scale of primitives */
GLfloat base_scale_x, base_scale_y, base_scale_z;
GLfloat tower_scale_x, tower_scale_y, tower_scale_z;
GLfloat body_scale_x, body_scale_y, body_scale_z;
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
	vx = 0; vx = 0, vz = 0;
	light_x = 2; light_y = 2; light_z = 2;
	angle_x = angle_y = angle_z = 0;
	angle_inc_x = angle_inc_y = angle_inc_z = 0; const float roughness = 0.8;
	model_scale = 1.f;
	aspect_ratio = 1.3333f;
	colourmode = 0;
	emitmode = 0;
	attenuationmode = 1; // Attenuation is on by default
	numlats = 40;		// Number of latitudes in our sphere
	numlongs = 40;		// Number of longitudes in our sphere

	camera_x = 0; camera_y = 1; camera_z = 5;

	origin_x = 0; origin_y = 0; origin_z = 0;
	base_x = 0; base_y = -1.f; base_z = 0;
	tower_x = 0; tower_y = -0.5f; tower_z = 0;
	body_x = 0; body_y = 0; body_z = 0;
	rotor_x = 0; rotor_y = 0; rotor_z = 0.25f;

	base_scale_x = 1.f; base_scale_y = 0.2f; base_scale_z = 1.f;
	tower_scale_x = 0.1f; tower_scale_y = 1.f; tower_scale_z = 0.1f;
	body_scale_x = 0.5f; body_scale_y = 0.5f; body_scale_z = 0.9f;
	rotor_scale_x = 0.115f; rotor_scale_y = 0.115f; rotor_scale_z = 0.15f;
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
		program_phong = glw->LoadShader("shaders\\phong.vert", "shaders\\phong.frag");
		program_oren_nayar = glw->LoadShader("shaders\\phong.vert", "shaders\\oren_nayar.frag");
	}
	catch (exception& e)
	{
		cout << "Caught exception: " << e.what() << endl;
		cin.ignore();
		exit(0);
	}

	/* Define the uniforms to send to the shaders */
	glUseProgram(program_phong);
	phong_modelID = glGetUniformLocation(program_phong, "model");
	phong_colourmodeID = glGetUniformLocation(program_phong, "colourmode");
	phong_emitmodeID = glGetUniformLocation(program_phong, "emitmode");
	phong_attenuationmodeID = glGetUniformLocation(program_phong, "attenuationmode");
	phong_viewID = glGetUniformLocation(program_phong, "view");
	phong_projectionID = glGetUniformLocation(program_phong, "projection");
	phong_lightposID = glGetUniformLocation(program_phong, "lightpos");
	phong_normalmatrixID = glGetUniformLocation(program_phong, "normalmatrix");

	glUseProgram(program_oren_nayar);
	oren_nayar_modelID = glGetUniformLocation(program_oren_nayar, "model");
	oren_nayar_colourmodeID = glGetUniformLocation(program_oren_nayar, "colourmode");
	oren_nayar_emitmodeID = glGetUniformLocation(program_oren_nayar, "emitmode");
	oren_nayar_attenuationmodeID = glGetUniformLocation(program_oren_nayar, "attenuationmode");
	oren_nayar_viewID = glGetUniformLocation(program_oren_nayar, "view");
	oren_nayar_projectionID = glGetUniformLocation(program_oren_nayar, "projection");
	oren_nayar_lightposID = glGetUniformLocation(program_oren_nayar, "lightpos");
	oren_nayar_normalmatrixID = glGetUniformLocation(program_oren_nayar, "normalmatrix");

	/* create our sphere and cube objects */
	baseCube.makeCube(vec4(0.3, 0.3, 0.28, 1.0));
	tower.makeCylinder(vec3(0.4, 0.6, 0.9));
	headCube.makeCube(vec4(0.4, 0.6, 0.9, 1.0));
	rotorSphere.makeSphere(numlats, numlongs, vec3(0.4, 0.6, 0.9));
	blade.makeBlade(vec4(0.4, 0.6, 0.9, 1.0));
}

/* Called to update the display. Note that this function is called in the event loop in the wrapper
   class because we registered display as a callback function */
void display()
{
	/* Define the background colour */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	/* Clear the colour and frame buffers */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Enable depth test */
	glEnable(GL_DEPTH_TEST);

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
		// Camera position
		vec3(camera_x, camera_y, camera_z),
		// Camera looks at the origin
		vec3(0, 0, 0),
		// Head is up (set to 0,-1,0 to look upside-down)
		vec3(0, 1, 0)
	);

	// Apply rotations to the view position. This wil get applied to the whole scene
	view = rotate(view, -radians(vx), vec3(1, 0, 0)); //rotating in clockwise direction around x-axis
	view = rotate(view, -radians(vy), vec3(0, 1, 0)); //rotating in clockwise direction around y-axis
	view = rotate(view, -radians(vz), vec3(0, 0, 1));

	// Define the light position and transform by the view matrix
	vec4 lightpos = view * vec4(light_x, light_y, light_z, 1.0);

	// Send the projection and view uniforms to the Phong shader
	glUseProgram(program_phong);
	glUniform1ui(phong_colourmodeID, colourmode);
	glUniformMatrix4fv(phong_viewID, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(phong_projectionID, 1, GL_FALSE, &projection[0][0]);
	glUniform4fv(phong_lightposID, 1, &lightpos[0]);
	glUniform1ui(phong_attenuationmodeID, attenuationmode);

	// Send the projection and view uniforms to the Oren Nayar shader
	glUseProgram(program_oren_nayar);
	glUniform1ui(oren_nayar_colourmodeID, colourmode);
	glUniformMatrix4fv(oren_nayar_viewID, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(oren_nayar_projectionID, 1, GL_FALSE, &projection[0][0]);
	glUniform4fv(oren_nayar_lightposID, 1, &lightpos[0]);
	glUniform1ui(oren_nayar_attenuationmodeID, attenuationmode);

	/* Draw a small sphere in the lightsource position to visually represent the light source */
	model.push(model.top());
	{
		glUseProgram(program_phong);
		model.top() = translate(model.top(), vec3(light_x, light_y, light_z));
		model.top() = scale(model.top(), vec3(0.05f, 0.05f, 0.05f)); // make a small sphere
		// Recalculate the normal matrix and send the model and normal matrices to the vertex shader
		glUniformMatrix4fv(phong_modelID, 1, GL_FALSE, &(model.top()[0][0]));
		normalmatrix = transpose(inverse(mat3(view * model.top())));
		glUniformMatrix3fv(phong_normalmatrixID, 1, GL_FALSE, &normalmatrix[0][0]);

		/* Draw our lightposition sphere with emit mode on*/
		emitmode = 1;
		glUniform1ui(phong_emitmodeID, emitmode);
		rotorSphere.drawSphere(drawmode);
		emitmode = 0;
		glUniform1ui(phong_emitmodeID, emitmode);
	}
	model.pop();

	// Define the global model transformations (rotate and scale). Note, we're not modifying the light source position
	model.top() = scale(model.top(), vec3(model_scale, model_scale, model_scale));
	model.top() = rotate(model.top(), -radians(angle_x), glm::vec3(1, 0, 0)); //rotating in clockwise direction around x-axis
	model.top() = rotate(model.top(), -radians(angle_y), glm::vec3(0, 1, 0)); //rotating in clockwise direction around y-axis
	model.top() = rotate(model.top(), -radians(angle_z), glm::vec3(0, 0, 1)); //rotating in clockwise direction around z-axis

	// This block of code draws the base cube
	model.push(model.top());
	{
		glUseProgram(program_oren_nayar);
		// Define the model transformations for the cube
		model.top() = translate(model.top(), vec3(base_x, base_y, base_z));
		model.top() = scale(model.top(), vec3(base_scale_x, base_scale_y, base_scale_z));

		// Send the model uniform and normal matrix to the currently bound shader,
		glUniformMatrix4fv(oren_nayar_modelID, 1, GL_FALSE, &(model.top()[0][0]));

		// Recalculate the normal matrix and send to the vertex shader
		normalmatrix = transpose(inverse(mat3(view * model.top())));
		glUniformMatrix3fv(oren_nayar_normalmatrixID, 1, GL_FALSE, &normalmatrix[0][0]);

		/* Draw our cube*/
		baseCube.drawCube(drawmode);
	}
	model.pop();

	// This block of code draws the tower cube
	model.push(model.top());
	{
		glUseProgram(program_phong);
		// Define the model transformations for the cylinder
		model.top() = translate(model.top(), vec3(tower_x, tower_y, tower_z));
		model.top() = scale(model.top(), vec3(tower_scale_x, tower_scale_y, tower_scale_z));

		// Send the model uniform and normal matrix to the currently bound shader,
		glUniformMatrix4fv(phong_modelID, 1, GL_FALSE, &(model.top()[0][0]));

		// Recalculate the normal matrix and send to the vertex shader
		normalmatrix = transpose(inverse(mat3(view * model.top())));
		glUniformMatrix3fv(phong_normalmatrixID, 1, GL_FALSE, &normalmatrix[0][0]);

		/* Draw our cylinder */
		tower.drawCylinder(drawmode);
	}
	model.pop();

	// This block of code draws the head cube
	model.push(model.top());
	{
		glUseProgram(program_phong);
		// Define the model transformations for the cube
		model.top() = translate(model.top(), vec3(body_x, body_y, body_z));
		model.top() = scale(model.top(), vec3(body_scale_x, body_scale_y, body_scale_z));

		// Send the model uniform and normal matrix to the currently bound shader,
		glUniformMatrix4fv(phong_modelID, 1, GL_FALSE, &(model.top()[0][0]));

		// Recalculate the normal matrix and send to the vertex shader
		normalmatrix = transpose(inverse(mat3(view * model.top())));
		glUniformMatrix3fv(phong_normalmatrixID, 1, GL_FALSE, &normalmatrix[0][0]);

		/* Draw our cube*/
		headCube.drawCube(drawmode);
	}
	model.pop();


	// This block of code draws the rotor sphere
	model.push(model.top());
	{
		glUseProgram(program_phong);
		model.top() = translate(model.top(), vec3(rotor_x, rotor_y, rotor_z));
		model.top() = rotate(model.top(), radians(rotation_z), glm::vec3(0, 0, 1));
		model.top() = scale(model.top(), vec3(rotor_scale_x, rotor_scale_y, rotor_scale_z));

		// Recalculate the normal matrix and send the model and normal matrices to the vertex shader
		glUniformMatrix4fv(phong_modelID, 1, GL_FALSE, &(model.top()[0][0]));
		normalmatrix = transpose(inverse(mat3(view * model.top())));
		glUniformMatrix3fv(phong_normalmatrixID, 1, GL_FALSE, &normalmatrix[0][0]);

		rotorSphere.drawSphere(drawmode);
	}
	model.pop();

	// This block of code draws the blade
	model.push(model.top());
	{
		glUseProgram(program_phong);
		// Transformations for the blade
		model.top() = rotate(model.top(), radians(rotation_z), glm::vec3(0, 0, 1));
		model.top() = translate(model.top(), vec3(origin_x, origin_y + 0.1f, origin_z + 0.26f));
		model.top() = scale(model.top(), vec3(model_scale, model_scale, model_scale));

		// Send the model uniform and normal matrix to the currently bound shader,
		glUniformMatrix4fv(oren_nayar_modelID, 1, GL_FALSE, &(model.top()[0][0]));

		// Recalculate the normal matrix and send to the vertex shader
		normalmatrix = transpose(inverse(mat3(view * model.top())));
		glUniformMatrix3fv(oren_nayar_normalmatrixID, 1, GL_FALSE, &normalmatrix[0][0]);

		/* Draw our blade*/
		blade.drawBlade(drawmode);
	}
	model.pop();

	// This block of code draws the blade
	model.push(model.top());
	{
		glUseProgram(program_phong);
		// Transformations for the blade
		model.top() = rotate(model.top(), radians(120.f + rotation_z), glm::vec3(0, 0, 1));
		model.top() = translate(model.top(), vec3(origin_x, origin_y + 0.1f, origin_z + 0.26f));
		model.top() = scale(model.top(), vec3(model_scale, model_scale, model_scale));

		// Send the model uniform and normal matrix to the currently bound shader,
		glUniformMatrix4fv(oren_nayar_modelID, 1, GL_FALSE, &(model.top()[0][0]));

		// Recalculate the normal matrix and send to the vertex shader
		normalmatrix = transpose(inverse(mat3(view * model.top())));
		glUniformMatrix3fv(oren_nayar_normalmatrixID, 1, GL_FALSE, &normalmatrix[0][0]);

		/* Draw our blade*/
		blade.drawBlade(drawmode);
	}
	model.pop();

	// This block of code draws the blade
	model.push(model.top());
	{
		glUseProgram(program_phong);
		// Transformations for the blade
		model.top() = rotate(model.top(), radians(240.f + rotation_z), glm::vec3(0, 0, 1));
		model.top() = translate(model.top(), vec3(origin_x, origin_y + 0.1f, origin_z + 0.26f));
		model.top() = scale(model.top(), vec3(model_scale, model_scale, model_scale));

		// Send the model uniform and normal matrix to the currently bound shader,
		glUniformMatrix4fv(oren_nayar_modelID, 1, GL_FALSE, &(model.top()[0][0]));

		// Recalculate the normal matrix and send to the vertex shader
		normalmatrix = transpose(inverse(mat3(view * model.top())));
		glUniformMatrix3fv(oren_nayar_normalmatrixID, 1, GL_FALSE, &normalmatrix[0][0]);

		/* Draw our blade*/
		blade.drawBlade(drawmode);
	}
	model.pop();

	glDisableVertexAttribArray(0);

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
	//if (action != GLFW_PRESS) return;

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

	// Light controls
	if (key == 'Z') light_x -= 0.05f;
	if (key == 'X') light_x += 0.05f;
	if (key == 'C') light_y -= 0.05f;
	if (key == 'V') light_y += 0.05f;
	if (key == 'B') light_z -= 0.05f;
	if (key == 'N') light_z += 0.05f;

	// Camera controls
	if (key == GLFW_KEY_UP) vx -= 1.f;
	if (key == GLFW_KEY_DOWN) vx += 1.f;
	if (key == GLFW_KEY_LEFT) vy -= 1.f;
	if (key == GLFW_KEY_RIGHT) vy += 1.f;
	if ((key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD) && camera_z > 1.f) camera_z -= 0.5f;
	if ((key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT) && camera_z < 15.f) camera_z += 0.5f;

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
}

void displayControls()
{
	cout << "Camera Controls" << endl;
	cout << "Rotate Camera Right: right arrow" << endl;
	cout << "Rotate Camera Left: left arrow" << endl;
	cout << "Move Camera Up: up arrow" << endl;
	cout << "Move Camera Down: down arrow" << endl;
	cout << "Zoom Camera In: plus" << endl;
	cout << "Zoom Camera Out: minus" << endl;
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

	displayControls();

	glw->eventLoop();

	delete(glw);
	return 0;
}