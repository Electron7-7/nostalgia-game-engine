// Simple Font Vertex Shader
#version 460 core
layout (location = 0) in vec3 _vertex_position;
layout (location = 1) in vec3 _vertex_color;
layout (location = 2) in vec3 _vertex_normal;
layout (location = 3) in vec2 _vertex_uv;

out vec2 vertex_uv;

uniform mat4 glyph_matrix;
uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

void main()
{
    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(_vertex_position.xy, 0.0f, 1.0f);
    vertex_uv   = vec2(_vertex_uv.x, 1.0f - _vertex_uv.y);
}
