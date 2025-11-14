#include "render_command.hpp"
#include "managers/theatre_manager.hpp"
#include "things/actors/actor.hpp"
#include "things/actors/nostalgia_player.hpp" // IWYU pragma: keep
#include "things/actors/light.hpp"
#include "things/devices/mesh_instance.hpp"
#include "things/devices/material.hpp"

RenderCommand::RenderCommand(std::shared_ptr<Actor> actor, const ID& shader_id):
    shader{shader_id},
    mesh_instance(actor->MeshInstanceID()),
    is_wireframe(actor->mWireframe),
    debug_highlight(actor->mDebugHighlight)
{
    if(dynamic_pointer_cast<light_t>(actor))
    {
        g_pTheatreManager->GetThing<Material>(
            g_pTheatreManager->GetThing<MeshInstance>(
                actor->MeshInstanceID()
                )->GetMaterialID()
            )->mColor = dynamic_pointer_cast<light_t>(actor)->mColor;
    }

    // https://www.reddit.com/r/opengl/comments/t01fwn/comment/hy7mezc
    glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), actor->Scale());
    glm::mat4 rotMat   = glm::mat4_cast(actor->Quaternion());
    glm::mat4 transMat = glm::translate(glm::mat4(1.0f), actor->Origin());
    model_matrix = transMat * rotMat * scaleMat;
}
