// Phong fragment shader

// Specify minimum OpenGL version
#version 400

// Global constants
vec4 global_ambient = vec4(0.04, 0.04, 0.04, 1.0);
int  shininess = 7;

// Inputs from the vertex shader
in vec3 vs_normal, vs_light_direction, vs_position;
in vec4 vs_diffuse_colour;

uniform uint emitmode;

// Output pixel fragment colour
out vec4 fs_output_colour;

void main()
{
	// Create a vec4(0, 0, 0) for our emmissive light but set to zero unless the emitmode flag is set
	vec4 emissive = vec4(0);		
	
	// Set the ambient colour
	vec4 fs_ambient_colour = vs_diffuse_colour * 0.2;

	// Set specular colour
	vec4 fs_specular_colour =  vec4(1.0, 0.8, 0.6, 1.0);

	// Set the distance to light source
	float distance_to_light = length(vs_light_direction);

	// Normalise interpolated vectors
	vec3 L = normalize(vs_light_direction);
	vec3 N = normalize(vs_normal);	

	// Calculate the diffuse component
	vec4 diffuse = max(dot(N, L), 0.0) * vs_diffuse_colour;

	// Calculate the specular component
	vec3 V = normalize(-vs_position);	
	vec3 R = reflect(-L, N);
	vec4 specular = pow(max(dot(R, V), 0.0), shininess) * fs_specular_colour;

	// Set the attenuation factor
	float attenuation = 1.0;
	
	// If emitmode is 1 enable emmissive lighting
	if (emitmode == 1) emissive = vec4(1.0, 1.0, 0.8, 1.0); 

	// Calculate the output colour, includung attenuation on the diffuse and specular components
	fs_output_colour = attenuation*(fs_ambient_colour + diffuse + specular) + emissive + global_ambient;
}