#include "nostalgia_player.hpp"
#include "theatre/parser/thing_data.hpp"
#include "events/event.hpp"
#include "settings/player.hpp"
#include "managers/input_manager.hpp"

void NostalgiaPlayer::SetVariables(Farg<ThingData> data)
{
    Actor::SetVariables(data);

    data.GetVariable(mViewPosition, "ViewPosition");
}

Shared<ThingData> NostalgiaPlayer::GetVariables() const
{
    Shared<ThingData> data{Actor::GetVariables()};

    data->AddVariable(mViewPosition, "ViewPosition");

    return data;
}

void NostalgiaPlayer::Input(InputEvent* event)
{}

void NostalgiaPlayer::Tick()
{
    Move({
        mCaptureKeyboard * (InputManager::IsActionDown("+right")   - InputManager::IsActionDown("+left")),
        mCaptureKeyboard * (InputManager::IsActionDown("+forward") - InputManager::IsActionDown("+backward"))
    });
    Look(InputManager::MouseMotion() * mCaptureMouse);

    glm::vec3 l_FrontBackVelocity = glm::vec3(Front()[0], 0.0f, Front()[2]) * (mMovementDirection[1] * Settings::Player::MovementSpeed);
    glm::vec3 l_LeftRightVelocity = Right() * (mMovementDirection[0] * Settings::Player::MovementSpeed);
    mVelocity = (l_FrontBackVelocity + l_LeftRightVelocity);
    mVelocity[1] = 0.0f;
    mOrigin += mVelocity;
    mMovementDirection = glm::vec3(0.0f);
    SetEuler(Euler(true) -= glm::vec3(mLookWish.y, mLookWish.x, 0.0f), true);
    mViewMatrix = glm::lookAt(mOrigin, mOrigin + Front(), Up());
}

void NostalgiaPlayer::Move(const glm::vec2& direction)
{
    mMovementDirection.x += direction.x;
    mMovementDirection.y += direction.y;
}

void NostalgiaPlayer::Look(const glm::vec2& motion)
{ mLookWish = motion * Settings::Player::MouseSensitivity * Settings::Player::MouseSensitivityScale; }

const glm::vec3& NostalgiaPlayer::ViewPosition() const
{ return mViewPosition; }

const glm::mat4& NostalgiaPlayer::ViewMatrix() const
{ return mViewMatrix; }

void NostalgiaPlayer::SetViewPosition(const glm::vec3& view_position)
{ mViewPosition = view_position; }
