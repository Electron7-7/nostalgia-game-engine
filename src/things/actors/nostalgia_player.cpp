#include "nostalgia_player.hpp"
#include "managers/theatre_manager.hpp"

Camera3D* NostalgiaPlayer::GetCamera()
{ return TheatreManager::GetThing<Camera3D>(m_CameraID).get(); }

void NostalgiaPlayer::SetOrigin(const glm::vec3& origin)
{
    Actor::SetOrigin(origin);
    GetCamera()->SetOrigin(origin);
}

void NostalgiaPlayer::SetRotation(const glm::quat& rotation)
{
    Actor::SetRotation(rotation);
    GetCamera()->SetRotation(rotation);
}
