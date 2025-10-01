#ifndef NOSTALGIA_PLAYER_H
#define NOSTALGIA_PLAYER_H

#include "actor.hpp"
#include "debug.hpp"

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
    virtual void SetupVariables(const data_t&);
    virtual bool Initialize();
    virtual void Input(const InputEvent&);
    virtual void Tick();

    void Move(const glm::vec2& Direction);
    void Look(const glm::vec2& Position);
    const glm::vec3& ViewPosition() const; // View position is an offset from the player's origin
    void SetViewPosition(const glm::vec3& ViewPosition);

private:
    DEBUG(friend void l_TerribleRenderDebugWindow();)
    glm::vec3 m_Velocity = glm::vec3(0.0f);
    glm::vec3 m_ViewPosition = glm::vec3(0.0f, 1.5f, 0.0f);
    glm::vec2 m_MovementDirection = glm::vec2(0.0f);
    glm::vec2 m_LookWish = glm::vec2(0.0f);
};

#endif // NOSTALGIA_PLAYER_H
