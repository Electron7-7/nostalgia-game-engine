#ifndef PLAYER_H
#define PLAYER_H

#include <Nostalgia/Nostalgia.hpp>
#include <Nostalgia/fwd/things.hpp>
#include <Nostalgia/things/thinkers/3d/actor_3d.hpp>

class EditorPlayer3D : public Actor3D
{
public:
    THING_CLASS(EditorPlayer3D, Actor3D)
    READY_OVERRIDE
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE
    TICK_OVERRIDE
    INPUT_OVERRIDE

    bool mCaptureMouse{false},
        mCaptureKeyboard{false};

protected:
    Shared<Collider3D> m_pCollider{nullptr};
    Shared<Camera3D> m_pCamera{nullptr};
    bool mEnableCollision{true};
    float mMouseSensitivity{0.15f}, mMouseSensitivityMultiplier{0.5f};
    glm::vec3 mVelocity{0.0f},
        mLookWish{0.0f},
        mMovementDirection{0.0f};
};

#endif // PLAYER_H
