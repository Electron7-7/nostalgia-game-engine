#ifndef RENDER_COMMAND_H
#define RENDER_COMMAND_H

#include "things/fwd.hpp"
#include "shader_interfaces/shader_interface.hpp"

#include <glm/glm.hpp>
#include <memory>

struct RenderCommand
{
public:
    RenderCommand(std::shared_ptr<Actor>, id_t ShaderID = Shaders::BlinnPhong);

    glm::vec3 ViewPosition() const;
    glm::mat4 ViewMatrix() const;
    glm::mat4 ProjectionMatrix() const;

    id_t m_ShaderID = Shaders::BlinnPhong;
    id_t m_MeshInstanceID = ID::None;
    glm::mat4 m_ModelMatrix = glm::mat4(1.0f);
    bool m_Wireframe = false;
};

#endif // RENDER_COMMAND_H
