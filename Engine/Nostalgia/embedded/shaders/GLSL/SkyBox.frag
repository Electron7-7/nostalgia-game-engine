// Blinn Phong Fragment Shader
#version 460 core

out vec4 FragColor;
in vec3 vertex_uv;

uniform samplerCube skybox;

void main()
{
    FragColor = texture(skybox, -vertex_uv);
}
