#ifndef RENDER_COMMAND_H
#define RENDER_COMMAND_H

#include "things/fwd.hpp"
#include "ids.hpp"
#include "shader_interfaces/shader_interface.hpp"

#include <glm/glm.hpp>
#include <memory>

struct RenderCommand
{
public:
    RenderCommand(std::shared_ptr<Actor>, ID ShaderID = Shaders::BlinnPhong);

    glm::vec3 ViewPosition() const;
    glm::mat4 ViewMatrix() const;
    glm::mat4 ProjectionMatrix() const;

    ID shader = Shaders::BlinnPhong;
    ID mesh_instance = ID::None;
    glm::mat4 model_matrix = glm::mat4(1.0f);
    bool is_wireframe = false;
    glm::vec4 debug_highlight{0.0f};
};

#endif // RENDER_COMMAND_H
