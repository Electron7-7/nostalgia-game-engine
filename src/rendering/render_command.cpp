#include "render_command.hpp"
#include "settings/settings.hpp"
#include "managers/theatre_manager.hpp"
#include "things/actors/nostalgia_player.hpp" // IWYU pragma: keep

#include <glm/ext/matrix_clip_space.hpp>

RenderCommand::RenderCommand(std::shared_ptr<Actor> actor, id_t shader)
: m_ShaderID(shader), m_MeshInstanceID(actor->GetMeshInstanceID())
{
    // https://www.reddit.com/r/opengl/comments/t01fwn/comment/hy7mezc
    glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), actor->cScale());
    glm::mat4 rotMat   = glm::mat4_cast(actor->Quaternion());
    glm::mat4 transMat = glm::translate(glm::mat4(1.0f), actor->cOrigin());
    m_ModelMatrix = transMat * rotMat * scaleMat;
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
    return glm::perspective(
        glm::radians(Settings::Player::FOV),
        Settings::Window::Width / static_cast<float>(Settings::Window::Height),
        Settings::Player::ViewCutoffNear,
        Settings::Player::ViewCutoffFar
    );
}
