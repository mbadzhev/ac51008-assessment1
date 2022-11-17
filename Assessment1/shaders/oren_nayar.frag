// Oren Nayar fragment shader

// Specify minimum OpenGL version
#version 400

// Global constants
vec4 global_ambient = vec4(0.04, 0.04, 0.04, 1.0);
int  shininess = 8;
const float PI = 3.141592653;
const float roughness = 0.99;

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
	vec4 fs_ambient_colour = vs_diffuse_colour * 0.2;
	
	// Normalise the input vectors
	vec3 normal = normalize(vs_normal);
    vec3 view_direction = normalize(-vs_position.xyz);
    vec3 light_direction = normalize(vs_light_direction);
	float distance_to_light = length(vs_light_direction);
	
	// Calculate these dot products which are used in the Oren Nayar equations
    float NdotL = dot(normal, light_direction);
    float NdotV = dot(normal, view_direction); 

    float angleVN = acos(NdotV);
    float angleLN = acos(NdotL);

	// Implement the Oren Nayar equations
    float alpha = max(angleVN, angleLN);
    float beta = min(angleVN, angleLN);
    float gamma = dot(view_direction - normal * dot(view_direction, normal), light_direction - normal * dot(light_direction, normal));

    float roughness_squared = roughness * roughness;
    float roughness_squared9 = (roughness_squared / (roughness_squared + 0.09));

    // Calculate C1, C2 and C3
    float C1 = 1.0 - 0.5 * (roughness_squared / (roughness_squared + 0.33));
    float C2 = 0.45 * roughness_squared9;

    if(gamma >= 0.0)
    {
        C2 *= sin(alpha);
    }
    else
    {
        C2 *= (sin(alpha) - pow((2.0 * beta) / PI, 3.0));
    }

    float pow_value = (4.0 * alpha * beta) / (PI * PI);
    float C3  = 0.125 * roughness_squared9 * pow_value * pow_value;

    // Calculate the main parts of the formula
    float A = gamma * C2 * tan(beta);
    float B = (1.0 - abs(gamma)) * C3 * tan((alpha + beta) / 2.0);

    // Combine the components
    float L1 = max(0.0, NdotL) * (C1 + A + B);

    // Calculate interreflection
    float twoBetaPi = 2.0 * beta / PI;

    float L2 = 0.17 * max(0.0, NdotL) * (roughness_squared / (roughness_squared + 0.13)) * (1.0 - gamma * twoBetaPi * twoBetaPi);

    vec4 oren_nayar = vec4(vs_diffuse_colour.xyz * (L1 + L2), 1.0);
	
	// Set the attenuation factor
	float attenuation = 1.0;
	
	// If emitmode is 1 then we enable emmissive lighting
	if (emitmode == 1) emissive = vec4(1.0, 1.0, 0.8, 1.0); 

	// Calculate the output colour, includung attenuation on the diffuse and specular components
	fs_output_colour = attenuation * (fs_ambient_colour + oren_nayar) + emissive + global_ambient;
}