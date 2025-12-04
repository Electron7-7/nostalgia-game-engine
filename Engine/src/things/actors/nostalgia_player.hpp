#ifndef NOSTALGIA_PLAYER_H
#define NOSTALGIA_PLAYER_H

#include "actor.hpp"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

enum class Movement
{
    Forward,
    Backward,
    Left,
    Right
};

// Actor type
class NostalgiaPlayer : public Actor
{
public:
    virtual void Tick();
    virtual void Input(InputEvent*);
    virtual void SetVariables(const ThingData&);
    virtual ThingData GetVariables() const;

    void Move(const glm::vec2& Direction);
    void Look(const glm::vec2& Position);
    const glm::vec3& ViewPosition() const; // View position is an offset from the player's origin
    const glm::mat4& ViewMatrix() const;
    void SetViewPosition(const glm::vec3& ViewPosition);

    bool mCaptureMouse{false};
    bool mCaptureKeyboard{false};

private:
    glm::vec3 mViewPosition = glm::vec3(0.0f, 1.5f, 0.0f);
    glm::mat4 mViewMatrix{glm::lookAt(mOrigin, mOrigin + Front(), Up())};
    glm::vec3 mVelocity = glm::vec3(0.0f);
    glm::vec2 mMovementDirection = glm::vec2(0.0f);
    glm::vec2 mLookWish = glm::vec2(0.0f);
};

#endif // NOSTALGIA_PLAYER_H
