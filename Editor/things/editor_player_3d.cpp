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
    mActivateOnStart = true;
    mMotion = MotionType::Kinematic;
}

void EditorPlayer3D::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);
    data.get_variable(m_pCamera, "Camera", "PlayerCamera");
    data.get_variable(mMouseSensitivity, "MouseSensitivity");
    data.get_variable(mMouseSensitivityMultiplier, "MouseSensitivityMultiplier", "MouseMultiplier");
}

Shared<ThingData> EditorPlayer3D::GetVariables() const
{
    Shared<ThingData> data{Super::GetVariables()};
    data->set_variable(m_pCamera, "Camera");
    data->set_variable(mMouseSensitivity, "MouseSensitivity");
    data->set_variable(mMouseSensitivityMultiplier, "MouseSensitivityMultiplier");

    return data;
}

void EditorPlayer3D::Ready()
{
    Super::Ready();

    bool _has_camera{false};
    for(ID child : Children())
    {
        if(ThingFactory::DerivedFrom(child, ThingType::Camera3D))
        {
            m_pCamera = ThingFactory::GetThing<Camera3D>(child);
            _has_camera = true;
            break;
        }
    }

    if(not _has_camera)
    {
        TheatreFile::ThingData cam_dat{ThingType::Camera3D, "EditorPlayer3D-Camera"};
        cam_dat.set_variable(glm::vec3{0.0f, 1.5f, 0.0f}, "Position");
        cam_dat.set_variable(true, "UseDefaultSkybox");
        cam_dat.set_variable(name(), "Parent");
        m_pCamera = ThingFactory::GetThing<Camera3D>(Theatre::Current()->CreateThing(cam_dat));

        SetPosition(Position());
        SetRotation(Rotation());
        SetScale(Scale());
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
    if(not mCaptureKeyboard and not mCaptureMouse)
        { return; }

    if(mCaptureMouse)
    {
        mLookWish = InputManager::MouseMotion().operator glm::vec2() * mMouseSensitivity * mMouseSensitivityMultiplier;
        m_pCamera->SetRotationDegrees(m_pCamera->RotationDegrees() - glm::vec3{mLookWish.y, mLookWish.x, 0.0f});
    }
    if(mCaptureKeyboard)
    {
        mMovementDirection.x = InputManager::IsActionDown("+right")   - InputManager::IsActionDown("+left");
        mMovementDirection.z = InputManager::IsActionDown("+forward") - InputManager::IsActionDown("+backward");
    }

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

    PhysicsEngine::Instance()->BodyInterface().SetLinearVelocity(id(), Math::Convert<JPH::Vec3>(mVelocity));
    Super::Tick();
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
