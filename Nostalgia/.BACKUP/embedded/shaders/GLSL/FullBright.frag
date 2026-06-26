// FullBright Fragment Shader
#version 460 core

out vec4 FragColor;

in vec3 fragment_position;
in vec3 vertex_normal;
in vec2 vertex_uv;
in vec3 vertex_color;

struct Material
{
	vec3 diffuse_color;
	sampler2D texture_diffuse;
	int use_diffuse;
};
uniform Material current_material;

uniform vec4 debug_highlight;

void main()
{
	vec3 use_texture_vec3 = vec3(1.0f - current_material.use_diffuse);
	vec3 material_out = vec3(texture(current_material.texture_diffuse, vertex_uv).rgb * current_material.use_diffuse) + use_texture_vec3;
	FragColor = vec4(material_out * current_material.diffuse_color * vertex_color, 1.0f) + debug_highlight;
}
