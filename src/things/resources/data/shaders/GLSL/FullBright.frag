// Blinn Phong Fragment Shader
#version 460 core

out vec4 FragColor;

in vec3 fragment_position;
in vec3 vertex_normal;
in vec2 vertex_uv;
in vec3 vertex_color;

struct Material
{
	#pragma message("(Todo) combine color and alpha into a vec4")
	vec3 diffuse_color;
	float alpha;
	sampler2D texture_diffuse;

	// Until I implement metallic, roughness, etc I'll just directly affect the specular
	float specular_strength;
	int specular_sharpness;
	sampler2D texture_specular;

	// These will eventually replace the specular variables, as they'll each affect the specular highlights in different ways
	// sampler2D texture_metallic;
	// float metallic_strength;
	// sampler2D texture_roughness;
	// float roughness_strength;
};

uniform Material current_material;

uniform vec4 debug_highlight;

uniform int debug_output;
#define ALL 0
#define COLOR 1
#define NORMAL 2
#define UV 3

void main()
{
	switch(debug_output)
	{
	case COLOR:
		FragColor = vec4(vertex_color, 1.0f);
		return;
	case NORMAL:
		FragColor = vec4(vertex_normal, 1.0f);
		return;
	case UV:
		FragColor = vec4(vertex_uv, 0.0f, 1.0f);
		return;
	case ALL:
	default:
		// FragColor = vec4(texture(current_material.texture_diffuse, vertex_uv).rgb * vertex_color, 1.0f) + debug_highlight;
		FragColor = vec4(texture(current_material.texture_diffuse, vertex_uv).rgb, 1.0f);
		return;
	}

}
