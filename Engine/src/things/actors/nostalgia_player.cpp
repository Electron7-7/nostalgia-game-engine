#include "nostalgia_player.hpp"
#include "camera.hpp" // IWYU pragma: keep // used by g_pTheatreManager->GetThing<Camera3D>
#include "core/uid.hpp"
#include "managers/theatre_manager.hpp"
#include "math/conversion.hpp"
#include "theatre/parser/thing_data.hpp"
#include "events/event.hpp"
#include "settings/player.hpp"
#include "managers/input_manager.hpp"
#include "things/devices/collider.hpp"
#include "physics/engine.hpp"

void NostalgiaPlayer::SetVariables(Farg<ThingData> data)
{
    Actor3D::SetVariables(data);

    data.GetVariable(mViewPosition, "ViewPosition");
    data.GetVariable(mCameraID, "Camera", "CameraID", "Camera3D");
    data.GetVariable(mColliderID, "Collider", "ColliderID", "PlayerCollider");
    data.GetVariable(Settings::Player::EnableGravity, "EnableGravity", "Gravity", "Fall");
}

Shared<ThingData> NostalgiaPlayer::GetVariables() const
{
    Shared<ThingData> data{Actor3D::GetVariables()};

    data->AddVariable(mViewPosition, "ViewPosition");
    data->AddVariable(mCameraID, "Camera");
    data->AddVariable(mColliderID, "Collider");
    data->AddVariable(Settings::Player::EnableGravity, "EnableGravity");

    return data;
}

void NostalgiaPlayer::Input(InputEvent* event)
{}

void NostalgiaPlayer::Ready()
{
    Actor3D::Ready();
    if(mScale == glm::vec3{1.0f})
        { mScale = glm::vec3{1.0f, 3.0f, 1.0f}; }
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
    if(mColliderID.invalid())
    {
        mColliderID = g_pTheatreManager->CreateThing({"DefaultPlayerCollider",
            ThingType::Collider,
            UID::Generate(),
            {
                {mOrigin + (mScale.y * 0.5f), "Origin"},
                {mQuaternion, "Quaternion"},
                {mScale, "Scale"},
                {MotionType::Kinematic, "Motion"},
                {ShapeType::Box, "Shape"},
            }});
    }
    add_child({mColliderID, ThingType::Collider}, true);
}

void NostalgiaPlayer::Tick()
{
    Move({
        mCaptureKeyboard * (InputManager::IsActionDown("+right")   - InputManager::IsActionDown("+left")),
        mCaptureKeyboard * (InputManager::IsActionDown("+forward") - InputManager::IsActionDown("+backward"))
    });
    Look(InputManager::MouseMotion() * mCaptureMouse);

    Euler(Euler(true) -= glm::vec3(0.0f, mLookWish.x, 0.0f), true);
    auto camera{g_pTheatreManager->GetThing<Camera3D>(mCameraID)};
    camera->Euler(camera->Euler(true) -= glm::vec3{mLookWish.y, mLookWish.x, 0.0f}, true);
    camera->Origin(mOrigin + mViewPosition);
    auto collider{g_pTheatreManager->GetThing<Collider>(mColliderID)};
    glm::vec3 l_FrontBackVelocity = Front() * (mMovementDirection[1] * Settings::Player::MovementSpeed);
    glm::vec3 l_LeftRightVelocity = Right() * (mMovementDirection[0] * Settings::Player::MovementSpeed);
    mVelocity = glm::clamp(l_FrontBackVelocity + l_LeftRightVelocity, -Settings::Player::MovementSpeed, Settings::Player::MovementSpeed);
    mVelocity[1] = 0.0f;
    PhysicsEngine::I()->BodyInterface().AddLinearVelocity(collider->id(),
        GlmToJolt<JPH::Vec3>(mVelocity));
#pragma message("FIXME: this is a shit way of doing this; change it to be on demand instead of every tick")
    mOrigin = collider->Origin();
    mMovementDirection = glm::vec3(0.0f);
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
