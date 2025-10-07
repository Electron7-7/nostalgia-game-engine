#include "render_command.hpp"
#include "settings/settings.hpp"
#include "managers/theatre_manager.hpp"
#include "things/actors/actor.hpp"
#include "things/actors/nostalgia_player.hpp" // IWYU pragma: keep
#include "things/actors/light.hpp"
#include "things/devices/mesh_instance.hpp"
#include "things/devices/material.hpp"

#include <glm/ext/matrix_clip_space.hpp>

RenderCommand::RenderCommand(std::shared_ptr<Actor> actor, ID shader_id)
: shader(shader_id), mesh_instance(actor->GetMeshInstanceID()), is_wireframe(actor->mWireframe)
{
    if(dynamic_pointer_cast<light_t>(actor))
    {
        TheatreManager::GetThing<Material>(
            TheatreManager::GetThing<MeshInstance>(
                actor->GetMeshInstanceID()
                )->GetMaterialID()
            )->mColor = dynamic_pointer_cast<light_t>(actor)->mColor;
    }

    // https://www.reddit.com/r/opengl/comments/t01fwn/comment/hy7mezc
    glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), actor->cScale());
    glm::mat4 rotMat   = glm::mat4_cast(actor->Quaternion());
    glm::mat4 transMat = glm::translate(glm::mat4(1.0f), actor->cOrigin());
    model_matrix = transMat * rotMat * scaleMat;
}

glm::vec3 RenderCommand::ViewPosition() const
{ return TheatreManager::GetLocalPlayer()->ViewPosition(); }

glm::mat4 RenderCommand::ViewMatrix() const
{
    const auto& Player = TheatreManager::GetLocalPlayer();
    const auto& Origin = Player->Origin();
    return glm::lookAt(Origin, Origin + Player->Front(), Player->Up());
}

glm::mat4 RenderCommand::ProjectionMatrix() const
{
    float aspect_ratio =
    (Settings::Window::Fullscreen)
        ? Settings::Window::FullscreenWidth / static_cast<float>(Settings::Window::FullscreenHeight)
        : Settings::Window::Width / static_cast<float>(Settings::Window::Height);

    return glm::perspective(
        glm::radians(Settings::Player::FOV),
        aspect_ratio,
        Settings::Player::ViewCutoffNear,
        Settings::Player::ViewCutoffFar
    );
}
