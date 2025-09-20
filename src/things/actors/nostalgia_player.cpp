#include "nostalgia_player.hpp"
#include "theatre/data_t.hpp"

void NostalgiaPlayer::SetupVariables(const data_t& data)
{
    Actor::SetupVariables(data);

    data.GetNumber(m_ViewPosition, "ViewPosition");
}

bool NostalgiaPlayer::Initialize()
{
    if(!Actor::Initialize())
        { return false; }
    return true;
}

const glm::vec3& NostalgiaPlayer::ViewPosition() const
{ return m_ViewPosition; }

void NostalgiaPlayer::SetViewPosition(const glm::vec3& view_position)
{ m_ViewPosition = view_position; }
