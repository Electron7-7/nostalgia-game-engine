// Blinn Phong Vertex Shader
#version 460 core
layout (location = 0) in vec3 _vertex_position;
layout (location = 1) in vec3 _vertex_normal;
layout (location = 2) in vec3 _vertex_color;
layout (location = 3) in vec2 _vertex_uv;

out vec3 fragment_position;
out vec3 vertex_normal;
out vec2 vertex_uv;
out vec3 vertex_color;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform mat3 normal_matrix;

void main()
{
	gl_Position = projection_matrix * view_matrix * model_matrix * vec4(_vertex_position, 1.0f);
	fragment_position = vec3(model_matrix * vec4(_vertex_position, 1.0f));
	vertex_normal = normal_matrix * _vertex_normal;
	vertex_uv = _vertex_uv;
	vertex_color = _vertex_color;
}
