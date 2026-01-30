// Simple Font Fragment Shader
#version 460 core

out vec4 FragColor;

in vec2 vertex_uv;

uniform sampler2D glyph;
uniform vec3 text_color;
uniform vec4 debug_highlight;
uniform int debug_solid;

void main()
{
    vec4 sampled = vec4(1.0f, 1.0f, 1.0f, texture(glyph, vertex_uv).r);
    if(debug_solid == 0 && sampled.a < 0.5f)
        discard;
    FragColor = vec4(text_color, 1.0f) * sampled + debug_highlight;
}
