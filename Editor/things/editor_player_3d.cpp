#include "./editor_player_3d.hpp"
#include <Nostalgia/application/application.hpp>
#include <Nostalgia/things/thing_data.hpp>
#include <Nostalgia/things/thinkers/3d/collider_3d.hpp>
#include <Nostalgia/things/thinkers/3d/camera_3d.hpp>
#include <Nostalgia/events/event.hpp>
#include <Nostalgia/managers/input_manager.hpp>
#include <Nostalgia/physics/engine.hpp>
#include <Nostalgia/settings/engine.hpp>
#include <Nostalgia/settings/world.hpp>
#include <Nostalgia/settings/player.hpp>
#include <Nostalgia/theatre/theatre.hpp>
#include <Nostalgia/things/thing_factory.hpp>

using namespace TheatreFile;

void EditorPlayer3D::InitVariables()
{
    Super::InitVariables();
}

void EditorPlayer3D::SetVariables(Farg<ThingData> data)
{
    Actor3D::SetVariables(data);

    data.get_variable(Settings::Player::EnableGravity, "EnableGravity", "Gravity", "Fall");
}

Shared<ThingData> EditorPlayer3D::GetVariables() const
{
    Shared<ThingData> data{Actor3D::GetVariables()};

    data->set_variable(Settings::Player::EnableGravity, "EnableGravity");

    return data;
}

void EditorPlayer3D::Ready()
{
    Actor3D::Ready();

    bool _has_collider{false}, _has_camera{false};
    for(ID child : Children())
    {
        if(_has_collider and _has_camera)
            { break; }
        if(ThingFactory::DerivedFrom(child, ThingType::Collider3D))
        {
            _has_collider = true;
            m_pCollider = ThingFactory::GetThing<Collider3D>(child);
        }
        else if(ThingFactory::DerivedFrom(child, ThingType::Camera3D))
        {
            _has_camera = true;
            m_pCamera = ThingFactory::GetThing<Camera3D>(child);
        }
    }

    if(not _has_collider)
    {
        TheatreFile::ThingData coll_dat{ThingType::Collider3D, "EditorPlayer3D_Collider"};
        coll_dat.set_variable(glm::vec3{1.0f, 3.0f, 1.0f}, "Scale");
        coll_dat.set_variable(MotionType::Kinematic, "Motion");
        coll_dat.set_variable(name(), "Parent");
        m_pCollider = ThingFactory::GetThing<Collider3D>(Theatre::Current()->CreateThing(coll_dat));
    }
    if(not _has_camera)
    {
        TheatreFile::ThingData cam_dat{ThingType::Camera3D, "EditorPlayer3D_Camera"};
        cam_dat.set_variable(glm::vec3{0.0f, 1.5f, 0.0f}, "Position");
        cam_dat.set_variable(true, "UseDefaultSkybox");
        cam_dat.set_variable(false, "Layer2");
        cam_dat.set_variable(name(), "Parent");
        m_pCamera = ThingFactory::GetThing<Camera3D>(Theatre::Current()->CreateThing(cam_dat));
    }

    Theatre::Current()->SetCurrentCamera(m_pCamera->uid());

    if(not Settings::Engine::IsEditorHint)
    {
        Application()->MainWindow()->SetMouseMode(IWindow::MOUSE_MODE_DISABLED);
        mCaptureMouse = mCaptureKeyboard = true;
    }
}

void EditorPlayer3D::Tick()
{
    if(mCaptureMouse)
    {
        mLookWish = InputManager::MouseMotion()
            * Settings::Player::MouseSensitivity * Settings::Player::MouseSensitivityScale;
        m_pCamera->SetRotationDegrees(m_pCamera->RotationDegrees() - glm::vec3{mLookWish.y, mLookWish.x, 0.0f});
    }

    if(mCaptureKeyboard)
    {
        mMovementDirection.x = InputManager::IsActionDown("+right")   - InputManager::IsActionDown("+left");
        mMovementDirection.z = InputManager::IsActionDown("+forward") - InputManager::IsActionDown("+backward");
    }
    else
        { return; }

    FAUTO _quaternion{m_pCamera->Quaternion()};

    glm::vec3 l_FrontBackVelocity{(_quaternion * Settings::World::Front()) *
        mMovementDirection.z * Settings::Player::MovementSpeed};
    glm::vec3 l_LeftRightVelocity{(_quaternion * Settings::World::Right()) *
        mMovementDirection.x * Settings::Player::MovementSpeed};
    glm::vec3 wish_velocity{l_FrontBackVelocity + l_LeftRightVelocity};

    for(int i{0}; i < 3; ++i)
    {
        if(wish_velocity[i] == mVelocity[i])
            { continue; }
        else if(wish_velocity[i] == 0.0f)
            { mVelocity[i] = 0.0f; }
        else
        {
            mVelocity[i] += wish_velocity[i] / Settings::Player::MovementAcceleration;
            if(glm::abs(mVelocity[i]) > glm::abs(wish_velocity[i]))
                { mVelocity[i] = wish_velocity[i]; }
        }
    }

    PhysicsEngine::Instance()->BodyInterface().SetLinearVelocity(m_pCollider->id(),
        Math::Convert<JPH::Vec3>(mVelocity));
    SetPosition(m_pCollider->Position());
}

void EditorPlayer3D::Input(InputEvent* inInput)
{
    if(inInput->IsJustPressed(Key::Escape))
    {
        mCaptureMouse = mCaptureKeyboard = not mCaptureMouse;
        Application()->MainWindow()->SetMouseMode((mCaptureMouse)
            ? IWindow::MOUSE_MODE_DISABLED
            : IWindow::MOUSE_MODE_VISIBLE);
    }
}
