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
    virtual void SetupVariables(const ThingData&);
    virtual bool Initialize();
    virtual void Input(const InputEvent&);
    virtual void Tick();

    void Move(const glm::vec2& Direction);
    void Look(const glm::vec2& Position);
    const glm::vec3& ViewPosition() const; // View position is an offset from the player's origin
    void SetViewPosition(const glm::vec3& ViewPosition);

private:
    glm::vec3 mVelocity = glm::vec3(0.0f);
    glm::vec3 mViewPosition = glm::vec3(0.0f, 1.5f, 0.0f);
    glm::vec2 mMovementDirection = glm::vec2(0.0f);
    glm::vec2 mLookWish = glm::vec2(0.0f);
};

#endif // NOSTALGIA_PLAYER_H
