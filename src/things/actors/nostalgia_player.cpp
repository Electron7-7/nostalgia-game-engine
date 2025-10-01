#include "nostalgia_player.hpp"
#include "theatre/data_t.hpp"
#include "input/event.hpp"
#include "settings/settings.hpp"
#include "managers/backend_manager.hpp"

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

void NostalgiaPlayer::Input(const InputEvent& event)
{
    Move({
        event.IsAction("+right")   - event.IsAction("+left"),
        event.IsAction("+forward") - event.IsAction("+backward")
    });
    if(g_pBackendManager->Windowing()->GetMouseMode() != MouseMode::Disabled)
        { m_LookWish = glm::vec2(0.0f); return; }
    else if(event.IsType(InputEventType::MouseMotion))
        { Look(event.MouseMotion()); }
}

void NostalgiaPlayer::Tick()
{
    glm::vec3 l_FrontBackVelocity = glm::vec3(Front()[0], 0.0f, Front()[2]) * (m_MovementDirection[1] * Settings::Player::MovementSpeed);
    glm::vec3 l_LeftRightVelocity = Right() * (m_MovementDirection[0] * Settings::Player::MovementSpeed);
    m_Velocity = (l_FrontBackVelocity + l_LeftRightVelocity);
    m_Velocity[1] = 0.0f;
    m_Origin += m_Velocity;
    m_MovementDirection = glm::vec3(0.0f);
    Euler(Euler(true) -= glm::vec3(m_LookWish.y, m_LookWish.x, 0.0f), true);
}

void NostalgiaPlayer::Move(const glm::vec2& direction)
{
    m_MovementDirection.x += direction.x;
    m_MovementDirection.y += direction.y;
}

void NostalgiaPlayer::Look(const glm::vec2& motion)
{ m_LookWish = motion * (Settings::Player::MouseSensitivity * Settings::Player::MouseSensitivityScale); }

const glm::vec3& NostalgiaPlayer::ViewPosition() const
{ return m_ViewPosition; }

void NostalgiaPlayer::SetViewPosition(const glm::vec3& view_position)
{ m_ViewPosition = view_position; }
