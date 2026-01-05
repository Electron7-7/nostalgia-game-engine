#include "nostalgia_player.hpp"
#include "camera.hpp" // IWYU pragma: keep // used by g_pTheatreManager->GetThing<Camera3D>
#include "core/uid.hpp"
#include "managers/theatre_manager.hpp"
#include "theatre/parser/thing_data.hpp"
#include "events/event.hpp"
#include "settings/player.hpp"
#include "managers/input_manager.hpp"
#include "things/devices/collider.hpp"

void NostalgiaPlayer::SetVariables(Farg<ThingData> data)
{
    Actor3D::SetVariables(data);

    data.GetVariable(mViewPosition, "ViewPosition");
    data.GetVariable(mCameraID, "Camera", "CameraID", "Camera3D");
    data.GetVariable(mColliderID, "Collider", "ColliderID", "PlayerCollider");
}

Shared<ThingData> NostalgiaPlayer::GetVariables() const
{
    Shared<ThingData> data{Actor3D::GetVariables()};

    data->AddVariable(mViewPosition, "ViewPosition");
    data->AddVariable(mCameraID, "Camera");
    data->AddVariable(mColliderID, "Collider");

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
                {PhysicsBodyMotion::Dynamic, "Motion"},
                {PhysicsBodyShape::Capsule, "Shape"},
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

    glm::vec3 l_FrontBackVelocity = Front() * (mMovementDirection[1] * Settings::Player::MovementSpeed);
    glm::vec3 l_LeftRightVelocity = Right() * (mMovementDirection[0] * Settings::Player::MovementSpeed);
    mVelocity = (l_FrontBackVelocity + l_LeftRightVelocity);
    mVelocity[1] = 0.0f;
    auto collider{g_pTheatreManager->GetThing<Collider>(mColliderID)};
    collider->SetLinearVelocity(mVelocity);
    mOrigin = DCast<Transform3D>(collider)->Origin();
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
