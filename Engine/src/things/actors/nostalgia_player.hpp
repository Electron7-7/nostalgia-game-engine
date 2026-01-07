#ifndef NOSTALGIA_PLAYER_H
#define NOSTALGIA_PLAYER_H

#include "things/actors/actor.hpp"

class NostalgiaPlayer : public Actor3D
{
public:
    virtual void Tick() override;
    virtual void Ready() override;
    virtual void Input(InputEvent*) override;
    virtual void SetVariables(Farg<ThingData>) override;
    virtual Shared<ThingData> GetVariables() const override;

    virtual void Move(Farg<glm::vec2> Direction);
    virtual void Look(Farg<glm::vec2> Position);
    virtual ID CameraID() const;
    virtual void CameraID(ID);

    virtual Farg<glm::vec3> Velocity() const;

    bool mCaptureMouse{false},
        mCaptureKeyboard{false};

protected:
    friend class ImGui_Debugger;
    glm::vec3 mViewPosition{0.0f, 1.5f, 0.0f},
        mVelocity{0.0f};
    glm::vec3 mMovementDirection{0.0f};
    glm::vec2 mLookWish{0.0f};
    ID mCameraID{};
    ID mColliderID{};
};

#endif // NOSTALGIA_PLAYER_H
