#ifndef NOSTALGIA_PLAYER_H
#define NOSTALGIA_PLAYER_H

#include "things/actors/actor.hpp"

class NostalgiaPlayer : public Actor
{
public:
    virtual void Tick() override;
    virtual void Ready() override;
    virtual void Input(InputEvent*) override;
    virtual void SetVariables(Farg<ThingData>) override;
    virtual Shared<ThingData> GetVariables() const override;

    void Move(Farg<glm::vec2> Direction);
    void Look(Farg<glm::vec2> Position);
    glm::vec3 ViewPosition() const; // View position is an offset from the player's origin
    void SetViewPosition(Farg<glm::vec3> ViewPosition);

    bool mCaptureMouse{false};
    bool mCaptureKeyboard{false};

    ID CameraID() const;
    void CameraID(ID);

private:
    glm::vec3 mViewPosition{0.0f, 1.5f, 0.0f};
    glm::vec3 mVelocity{0.0f};
    glm::vec2 mMovementDirection{0.0f};
    glm::vec2 mLookWish{0.0f};
    ID mCameraID{};
};

#endif // NOSTALGIA_PLAYER_H
