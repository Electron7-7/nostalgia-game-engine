#ifdef FWD_DCL
    struct RenderCommand;
#elif !defined RENDER_COMMAND_H
#define RENDER_COMMAND_H

#define FWD_DCL
#   include "things/actors/actor.hpp"
#undef  FWD_DCL

#include "core/id.hpp"
#include "shader.hpp"

#include <glm/glm.hpp>

struct RenderCommand
{
public:
    RenderCommand(Shared<Actor>, ID ShaderID = Shaders::BlinnPhong);

    ID shader{Shaders::BlinnPhong};
    ID mesh_instance{ID::Invalid};
    glm::mat4 model_matrix{1.0f};
    bool is_wireframe{false};
    glm::vec4 debug_highlight{0.0f};
};

#endif // RENDER_COMMAND_H
