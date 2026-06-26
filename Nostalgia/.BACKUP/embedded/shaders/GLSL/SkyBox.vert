// Skybox Vertex Shader
#version 460 core
layout (location = 0) in vec3 _vertex_position;
layout (location = 1) in vec3 _vertex_color;
layout (location = 2) in vec3 _vertex_normal;
layout (location = 3) in vec2 _vertex_uv;

out vec3 vertex_uv;

uniform mat4 projection_matrix;
uniform mat4 view_matrix;

void main()
{
    vertex_uv = _vertex_position;
    gl_Position = projection_matrix * view_matrix * vec4(_vertex_position, 1.0f);
}
