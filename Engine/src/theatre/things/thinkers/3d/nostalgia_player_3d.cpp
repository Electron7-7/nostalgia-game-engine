#include "nostalgia_player_3d.hpp"
#include "camera_3d.hpp" // IWYU pragma: keep // used by g_pTheatreManager->GetThinker<Camera3D>
#include "core/uid.hpp"
#include "managers/theatre_manager.hpp"
#include "math/conversion.hpp"
#include "settings/world.hpp"
#include "theatre/parser/thing_data.hpp"
#include "events/event.hpp"
#include "settings/player.hpp"
#include "managers/input_manager.hpp"
#include "theatre/things/thinkers/3d/collider_3d.hpp"
#include "physics/engine.hpp"

void NostalgiaPlayer3D::SetVariables(Farg<ThingData> data)
{
    Actor3D::SetVariables(data);

    data.GetVariable(mViewPosition, "ViewPosition");
    data.GetVariable(mCameraID, "Camera", "CameraID", "Camera3D");
    data.GetVariable(mColliderID, "Collider", "ColliderID", "PlayerCollider");
    data.GetVariable(Settings::Player::EnableGravity, "EnableGravity", "Gravity", "Fall");
}

Shared<ThingData> NostalgiaPlayer3D::GetVariables() const
{
    Shared<ThingData> data{Actor3D::GetVariables()};

    data->AddVariable(mViewPosition, "ViewPosition");
    data->AddVariable(mCameraID, "Camera");
    data->AddVariable(mColliderID, "Collider");
    data->AddVariable(Settings::Player::EnableGravity, "EnableGravity");

    return data;
}

void NostalgiaPlayer3D::Input(InputEvent* event)
{}

void NostalgiaPlayer3D::Ready()
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
            ThingType::Collider3D,
            UID::Generate(),
            {
                {mPosition, "Origin"},
                {mQuaternion, "Quaternion"},
                {mScale, "Scale"},
                {MotionType::Kinematic, "Motion"},
                {ShapeType::Box, "Shape"},
            }});
    }
    add_child({mColliderID, ThingType::Collider3D}, true);
}

void NostalgiaPlayer3D::Tick()
{
    Move({
        mCaptureKeyboard * (InputManager::IsActionDown("+right")   - InputManager::IsActionDown("+left")),
        mCaptureKeyboard * (InputManager::IsActionDown("+forward") - InputManager::IsActionDown("+backward"))
    });
    Look(InputManager::MouseMotion() * mCaptureMouse);

    SetRotationDegrees(mEulerRotationDegrees -= glm::vec3{0.0f, mLookWish.x, 0.0f});
    auto camera{g_pTheatreManager->GetThinker<Camera3D>(mCameraID)};
    camera->SetRotationDegrees(camera->RotationDegrees() - glm::vec3{mLookWish.y, mLookWish.x, 0.0f});
    camera->SetPosition(mPosition + mViewPosition);

    auto collider{g_pTheatreManager->GetThinker<Collider3D>(mColliderID)};

    glm::vec3 l_FrontBackVelocity{(mQuaternion * Settings::World::Front()) *
        mMovementDirection.z * Settings::Player::MovementSpeed};
    glm::vec3 l_LeftRightVelocity{(mQuaternion * Settings::World::Right()) *
        mMovementDirection.x * Settings::Player::MovementSpeed};
    glm::vec3 wish_velocity{l_FrontBackVelocity + l_LeftRightVelocity};

    for(int i{0}; i < 3; ++i)
    {
        if(wish_velocity[i] == mVelocity[i])
            { continue; }
        else if(wish_velocity[i] == 0.0f)
        {
            mVelocity[i] *= mVelocity[i] / Settings::Player::MovementAcceleration;
            if(glm::abs(mVelocity[i]) <= 0.001f)
                { mVelocity[i] = 0.0f; }
        }
        else
        {
            mVelocity[i] += wish_velocity[i] / Settings::Player::MovementAcceleration;
            if(glm::abs(mVelocity[i]) > glm::abs(wish_velocity[i]))
                { mVelocity[i] = wish_velocity[i]; }
        }
    }
    mVelocity[1] = 0.0f;

    PhysicsEngine::Inst()->BodyInterface().SetLinearVelocity(collider->id(),
        GlmToJolt<JPH::Vec3>(mVelocity));
    mPosition = collider->Position();
}

ID NostalgiaPlayer3D::CameraID() const
{ return mCameraID; }

void NostalgiaPlayer3D::SetCameraID(ID inID)
{ mCameraID = inID; }

Farg<glm::vec3> NostalgiaPlayer3D::Velocity() const
{ return mVelocity; }

void NostalgiaPlayer3D::Move(const glm::vec2& direction)
{
    mMovementDirection.x = direction.x;
    mMovementDirection.z = direction.y;
}

void NostalgiaPlayer3D::Look(const glm::vec2& motion)
{ mLookWish = motion * Settings::Player::MouseSensitivity * Settings::Player::MouseSensitivityScale; }
