#include "nostalgia_player.hpp"
#include "camera_3d.hpp" // IWYU pragma: keep // used by g_pTheatreManager->GetThing<Camera3D>
#include "core/uid.hpp"
#include "managers/theatre_manager.hpp"
#include "theatre/parser/thing_data.hpp"
#include "events/event.hpp"
#include "settings/player.hpp"
#include "managers/input_manager.hpp"

void NostalgiaPlayer::SetVariables(Farg<ThingData> data)
{
    Actor::SetVariables(data);

    data.GetVariable(mViewPosition, "ViewPosition");
    data.GetVariable(mCameraID, "Camera", "CameraID", "Camera3D");
}

Shared<ThingData> NostalgiaPlayer::GetVariables() const
{
    Shared<ThingData> data{Actor::GetVariables()};

    data->AddVariable(mViewPosition, "ViewPosition");
    data->AddVariable(mCameraID, "Camera");

    return data;
}

void NostalgiaPlayer::Input(InputEvent* event)
{}

void NostalgiaPlayer::Ready()
{
    Actor::Ready();
    if(mCameraID.invalid())
    {
        mCameraID = g_pTheatreManager->CreateThing({"DefaultPlayerCam",
            ThingType::Camera3D,
            UID::Generate(),
            {
                {mViewPosition, "Origin"},
                {mQuaternion, "Quaternion"},
                {true, "Current"},
                {true, "UseDefaultSkybox"},
            }});
    }
}

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
    auto camera{g_pTheatreManager->GetThing<Camera3D>(mCameraID)};
    camera->SetOrigin(mOrigin + mViewPosition);
    camera->SetQuaternion(mQuaternion);
}

void NostalgiaPlayer::Move(const glm::vec2& direction)
{
    mMovementDirection.x += direction.x;
    mMovementDirection.y += direction.y;
}

void NostalgiaPlayer::Look(const glm::vec2& motion)
{ mLookWish = motion * Settings::Player::MouseSensitivity * Settings::Player::MouseSensitivityScale; }

ID NostalgiaPlayer::CameraID() const
{ return mCameraID; }

void NostalgiaPlayer::CameraID(ID inID)
{ mCameraID = inID; }
