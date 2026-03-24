#ifndef PLAYER_H
#define PLAYER_H

#include <Nostalgia/Nostalgia.hpp>
#include <Nostalgia/things/thinkers/3d/nostalgia_player_3d.hpp>

class EditorPlayer3D : public NostalgiaPlayer3D
{
public:
    SUPER(NostalgiaPlayer3D)
    READY_OVERRIDE
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    virtual void Tick() override;

    bool mCaptureMouse{false},
        mCaptureKeyboard{false};

protected:
    ID mMainColliderID{};
    bool mEnableCollision{true};
    glm::vec3 mVelocity{0.0f},
        mLookWish{0.0f},
        mMovementDirection{0.0f};
};

#endif // PLAYER_H
