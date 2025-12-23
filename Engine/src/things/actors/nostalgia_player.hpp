#ifdef FWD_DCL
    class NostalgiaPlayer;
#elif !defined NOSTALGIA_PLAYER_H
#define NOSTALGIA_PLAYER_H

#include "actor.hpp"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

class NostalgiaPlayer : public Actor
{
public:
    virtual void Tick() override;
    virtual void Input(InputEvent*) override;
    virtual void SetVariables(Farg<ThingData>) override;
    virtual Shared<ThingData> GetVariables() const override;

    void Move(Farg<glm::vec2> Direction);
    void Look(Farg<glm::vec2> Position);
    Farg<glm::vec3> ViewPosition() const; // View position is an offset from the player's origin
    Farg<glm::mat4> ViewMatrix() const;
    void SetViewPosition(Farg<glm::vec3> ViewPosition);

    bool mCaptureMouse{false};
    bool mCaptureKeyboard{false};

private:
    glm::vec3 mViewPosition{0.0f, 1.5f, 0.0f};
    glm::mat4 mViewMatrix{glm::lookAt(mOrigin, mOrigin + Front(), Up())};
    glm::vec3 mVelocity{0.0f};
    glm::vec2 mMovementDirection{0.0f};
    glm::vec2 mLookWish{0.0f};
};

#endif // NOSTALGIA_PLAYER_H
