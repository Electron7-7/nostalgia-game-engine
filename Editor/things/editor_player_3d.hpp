#ifndef PLAYER_H
#define PLAYER_H

#include <Nostalgia/Nostalgia.hpp>
#include <Nostalgia/fwd/things.hpp>
#include <Nostalgia/things/thinkers/3d/actor_3d.hpp>

class EditorPlayer3D : public Actor3D
{
public:
    SET_SUPER(Actor3D)
    DEFINE_TYPEID(EditorPlayer3D, __editorplayer3d)
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
    glm::vec3 mVelocity{0.0f},
        mLookWish{0.0f},
        mMovementDirection{0.0f};
};

#endif // PLAYER_H
