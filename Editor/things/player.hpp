#ifndef PLAYER_H
#define PLAYER_H

#include <Nostalgia/Nostalgia.hpp>
#include <Nostalgia/things/thinkers/3d/nostalgia_player_3d.hpp>

class EditorPlayer3D : public NostalgiaPlayer3D
{
public:
    virtual void Tick() override;
    virtual void Ready() override;
    virtual void SetVariables(Farg<TheatreFile::ThingData>) override;
    virtual Shared<TheatreFile::ThingData> GetVariables() const override;

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
