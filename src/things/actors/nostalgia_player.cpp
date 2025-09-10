#include "nostalgia_player.hpp"
#include "managers/theatre_manager.hpp"

void NostalgiaPlayer::SetupVariables(const data_t& data)
{
    Actor::SetupVariables(data);
}

bool NostalgiaPlayer::Initialize()
{
    GetCamera()->SetOrigin(Origin());
    GetCamera()->SetRotation(Rotation());
    // return Actor::Initialize(); // Is this required or would it break shit?
    return true;
}

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
