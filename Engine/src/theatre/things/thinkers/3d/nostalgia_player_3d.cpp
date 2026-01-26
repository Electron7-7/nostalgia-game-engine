#include "nostalgia_player_3d.hpp"
#include "camera_3d.hpp" // IWYU pragma: keep // used by g_pTheatreManager->CurrentTheatre()->GetThinker<Camera3D>
#include "managers/theatre_manager.hpp"
#include "math/conversion.hpp"
#include "settings/engine.hpp"
#include "settings/world.hpp"
#include "events/event.hpp"
#include "settings/player.hpp"
#include "managers/input_manager.hpp"
#include "theatre/things/thinkers/3d/collider_3d.hpp"
#include "theatre/things/thinkers/3d/camera_3d.hpp"
#include "theatre/things/thinkers/3d/visual_3d.hpp"
#include "physics/engine.hpp"
#include "theatre/parser.hpp"

using namespace TheatreFile;

void NostalgiaPlayer3D::SetVariables(Farg<ThingData> data)
{
    Actor3D::SetVariables(data);

    if(!data.get_variable(mScale, "Scale", "Size", "OuuughImSoBigAndRound"))
        { mScale = glm::vec3{1.0f, 3.0f, 1.0f}; }
    data.get_variable(mViewPosition, "ViewPosition");
    data.get_variable(mCameraID, "Camera", "CameraID", "Camera3D");
    data.get_variable(mColliderID, "Collider", "ColliderID", "PlayerCollider");
    data.get_variable(Settings::Player::EnableGravity, "EnableGravity", "Gravity", "Fall");
}

Shared<ThingData> NostalgiaPlayer3D::GetVariables() const
{
    Shared<ThingData> data{Actor3D::GetVariables()};

    data->set_variable(mViewPosition, "ViewPosition");
    data->set_variable(mCameraID, "Camera");
    data->set_variable(mColliderID, "Collider");
    data->set_variable(Settings::Player::EnableGravity, "EnableGravity");

    return data;
}

void NostalgiaPlayer3D::Input(InputEvent* event)
{}

void NostalgiaPlayer3D::Ready()
{
    Actor3D::Ready();
    if(mCameraID.invalid())
    {
        TheatreFile::ThingData cam_dat{ThingType::Camera3D, "DefaultPlayerCam"};
        cam_dat.set_variable(glm::vec3{0.0f, 2.0f, -1.0f}, "Position");
        cam_dat.set_variable(true, "Current");
        cam_dat.set_variable(true, "UseDefaultSkybox");
        mCameraID = m_pRootTheatre->CreateThing(cam_dat);
        m_pRootTheatre->SetParent(mCameraID, mUID);
        BitMask cam_layers{};
        cam_layers.disable(1);
        my_theatre()->SetParent(mCameraID, mUID);
    }
    if(mColliderID.invalid())
    {
        TheatreFile::ThingData coll_dat{ThingType::Collider3D, "DefaultPlayerCollider"};
        coll_dat.set_variable(mPosition, "Origin");
        coll_dat.set_variable(mQuaternion, "Quaternion");
        coll_dat.set_variable(mScale, "Scale");
        coll_dat.set_variable(MotionType::Kinematic, "Motion");
        coll_dat.set_variable(ShapeType::Box, "Shape");
        mColliderID = my_theatre()->CreateThing(coll_dat);
        my_theatre()->SetParent(mColliderID, mUID);
        mScale = glm::vec3{1.0f};
    }
    if(Settings::Engine::IsEditorHint)
    {
        TheatreFile::ThingData axis_dat{ThingType::MeshInstance3D, "PlayerDebugAxisMesh"};
        axis_dat.set_variable(mUID, "Parent");
        axis_dat.set_variable(UID::m_DebugAxis, "Mesh");
        ID axis_mesh{};
        m_pRootTheatre->SetParent(axis_mesh = m_pRootTheatre->CreateThing(axis_dat), mUID);
        BitMask layers{false};
        layers.enable(1);
        my_theatre()->SetParent(my_theatre()->CreateThing(axis_dat), mUID);
    }
}

void NostalgiaPlayer3D::Tick()
{
    Move({
        mCaptureKeyboard * (InputManager::IsActionDown("+right")   - InputManager::IsActionDown("+left")),
        mCaptureKeyboard * (InputManager::IsActionDown("+forward") - InputManager::IsActionDown("+backward"))
    });
    Look(InputManager::MouseMotion() * mCaptureMouse);

    SetRotationDegrees(RotationDegrees() - glm::vec3{0.0f, mLookWish.x, 0.0f});
    auto camera{my_theatre()->GetThinker<Actor3D>(mCameraID)};
    camera->SetRotationDegrees(camera->RotationDegrees() - glm::vec3{mLookWish.y, 0.0f, 0.0f});

    auto collider{g_pTheatreManager->CurrentTheatre()->GetThinker<Collider3D>(mColliderID)};

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

Farg<glm::vec3> NostalgiaPlayer3D::Velocity() const
{ return mVelocity; }

void NostalgiaPlayer3D::Move(const glm::vec2& direction)
{
    mMovementDirection.x = direction.x;
    mMovementDirection.z = direction.y;
}

void NostalgiaPlayer3D::Look(const glm::vec2& motion)
{ mLookWish = motion * Settings::Player::MouseSensitivity * Settings::Player::MouseSensitivityScale; }
