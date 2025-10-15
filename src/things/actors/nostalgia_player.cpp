#include "nostalgia_player.hpp"
#include "theatre_parser/thing_data.hpp"
#include "input/event.hpp"
#include "settings/settings.hpp"
#include "managers/backend_manager.hpp"

void NostalgiaPlayer::SetVariables(const ThingData& data)
{
    Actor::SetVariables(data);

    data.GetVariable(mViewPosition, "ViewPosition");
}

ThingData NostalgiaPlayer::GetVariables() const
{
    ThingData data{Actor::GetVariables()};

    data.AddVariable(mViewPosition, "ViewPosition");

    return data;
}


void NostalgiaPlayer::Input(const InputEvent& event)
{
    Move({
        event.IsAction("+right")   - event.IsAction("+left"),
        event.IsAction("+forward") - event.IsAction("+backward")
    });
    if(g_pBackendManager->Windowing()->GetMouseMode() != MouseMode::Disabled)
        { mLookWish = glm::vec2(0.0f); return; }
    else if(event.IsType(InputEventType::MouseMotion))
        { Look(event.MouseMotion()); }
}

void NostalgiaPlayer::Tick()
{
    glm::vec3 l_FrontBackVelocity = glm::vec3(Front()[0], 0.0f, Front()[2]) * (mMovementDirection[1] * Settings::Player::MovementSpeed);
    glm::vec3 l_LeftRightVelocity = Right() * (mMovementDirection[0] * Settings::Player::MovementSpeed);
    mVelocity = (l_FrontBackVelocity + l_LeftRightVelocity);
    mVelocity[1] = 0.0f;
    mOrigin += mVelocity;
    mMovementDirection = glm::vec3(0.0f);
    SetEuler(Euler(true) -= glm::vec3(mLookWish.y, mLookWish.x, 0.0f), true);
}

void NostalgiaPlayer::Move(const glm::vec2& direction)
{
    mMovementDirection.x += direction.x;
    mMovementDirection.y += direction.y;
}

void NostalgiaPlayer::Look(const glm::vec2& motion)
{ mLookWish = motion * (Settings::Player::MouseSensitivity * Settings::Player::MouseSensitivityScale); }

const glm::vec3& NostalgiaPlayer::ViewPosition() const
{ return mViewPosition; }

void NostalgiaPlayer::SetViewPosition(const glm::vec3& view_position)
{ mViewPosition = view_position; }
