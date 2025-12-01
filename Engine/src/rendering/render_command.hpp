#ifndef RENDER_COMMAND_H
#define RENDER_COMMAND_H

#include "things/fwd.hpp"
#include "core/id.hpp"
#include "shader.hpp"

#include <glm/glm.hpp>
#include <memory>

struct RenderCommand
{
public:
    RenderCommand(std::shared_ptr<Actor>, FARG(ID) ShaderID = Shaders::BlinnPhong);

    ID shader{Shaders::BlinnPhong};
    ID mesh_instance{ID::Invalid};
    glm::mat4 model_matrix{1.0f};
    bool is_wireframe{false};
    glm::vec4 debug_highlight{0.0f};
};

#endif // RENDER_COMMAND_H
