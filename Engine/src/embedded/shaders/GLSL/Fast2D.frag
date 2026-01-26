// Fast 2D Fragment Shader
#version 460 core

out vec4 FragColor;

in vec2 vertex_uv;
in vec3 vertex_color;

uniform sampler2D sprite_texture;
uniform vec3 sprite_color;
uniform int use_texture;
uniform vec4 debug_highlight;

void main()
{ FragColor = vec4(vec3(texture(sprite_texture, vertex_uv).rgb * use_texture) + vec3(1.0f - use_texture) * sprite_color, 1.0f) + debug_highlight; }
