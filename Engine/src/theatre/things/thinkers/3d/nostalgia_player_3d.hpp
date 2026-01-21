#ifndef NOSTALGIA_PLAYER_H
#define NOSTALGIA_PLAYER_H

#include "theatre/things/thinkers/3d/actor_3d.hpp"

class NostalgiaPlayer3D : public Actor3D
{
public:
    virtual void Tick() override;
    virtual void Ready() override;
    virtual void Input(InputEvent*) override;
    virtual void SetVariables(Farg<TheatreFile::ThingData>) override;
    virtual Shared<TheatreFile::ThingData> GetVariables() const override;

#pragma message("TODO: get rid of hardcoded camera and collider code")
    virtual ID CameraID() const;

    virtual Farg<glm::vec3> Velocity() const;

    virtual void Move(Farg<glm::vec2> Direction);
    virtual void Look(Farg<glm::vec2> Position);

    bool mCaptureMouse{false},
        mCaptureKeyboard{false};

protected:
    glm::vec3 mViewPosition{0.0f, 1.5f, 0.0f},
        mVelocity{0.0f};
    glm::vec3 mMovementDirection{0.0f};
    glm::vec2 mLookWish{0.0f};
    ID mCameraID{};
    ID mColliderID{};
};

#endif // NOSTALGIA_PLAYER_H
