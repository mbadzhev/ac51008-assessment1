// Phong vertex shader

// Specify minimum OpenGL version
#version 400

// Vertex attributes
layout(location = 0) in vec3 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec3 normal;

// Outputs to fragment the shader
out vec3 final_position, final_normal, final_light_direction;
out vec4 final_diffuse_colour;

// Uniforms defined in the application
uniform mat4 model, view, projection;
uniform mat3 normalmatrix;
uniform uint colourmode;
uniform vec4 lightpos;

void main()
{
	// Convert the (x,y,z) position to homogeneous coordinates (x,y,z,w)
	vec4 position_homogeneous = vec4(position, 1.0);
	
	// Convert the light position to three dimentions (x, y, z)
	vec3 light_position = lightpos.xyz;

	// Switch the vertex colour based on the colourmode
	if (colourmode == 1)
		final_diffuse_colour = colour;
	else
		final_diffuse_colour = vec4(1.0, 0, 0, 1.0);

	/* Define vectors for calculating diffuse and specular lighting */
	// Calculate the model-view transformation
	mat4 model_view_matrix = view * model;

	// Transform the vertex position into eye-space in three dimentions (x, y, z)
	final_position = (model_view_matrix * position_homogeneous).xyz;

	// Modify the normal by the normal-matrix, transforming it to eye-space
	final_normal = normalize(normalmatrix * normal);

	// Calculate the vector from the light position to the vertex in eye space
	final_light_direction = light_position - final_position;			

	// Calculate the vertex position in projection space and output to the pipleline
	gl_Position = (projection * view * model) * position_homogeneous;
}