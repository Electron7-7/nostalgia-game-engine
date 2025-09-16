#ifndef NOSTALGIA_PLAYER_H
#define NOSTALGIA_PLAYER_H

#include "actor.hpp"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

// Actor type
class NostalgiaPlayer : public Actor
{
public:
    virtual void SetupVariables(const data_t&);
    virtual bool Initialize();

    // View position is an offset from the player's origin
    const glm::vec3& ViewPosition() const;
    void SetViewPosition(const glm::vec3& ViewPosition);

private:
    glm::vec3 m_ViewPosition = glm::vec3(0.0f, 1.5f, 0.0f);
};

#endif // NOSTALGIA_PLAYER_H
