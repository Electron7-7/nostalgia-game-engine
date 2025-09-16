#include "render_command.hpp"
#include "settings/settings.hpp"
#include "managers/theatre_manager.hpp"
#include "things/actors/nostalgia_player.hpp" // IWYU pragma: keep
#include "things/resources/complex/material.hpp"

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>

RenderCommand::RenderCommand()
: m_IsSafe(false), m_RenderContext(RenderContext())
{}

RenderCommand::RenderCommand(RenderContext context, id_t mesh_id, std::shared_ptr<Material> mat)
: m_IsSafe(true), m_RenderContext(context), m_MeshID(mesh_id), m_Material(mat)
{}

const bool& RenderCommand::IsSafe() const
{ return m_IsSafe; }

const RenderContext& RenderCommand::GetRenderContext() const
{ return m_RenderContext; }

const glm::vec3& RenderCommand::ViewPosition() const
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

id_t RenderCommand::GetMeshID() const
{ return m_MeshID; }

std::shared_ptr<Material> RenderCommand::GetMaterial() const
{ return m_Material; }
