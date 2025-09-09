#ifndef ACTOR_H
#define ACTOR_H

#include "thing.hpp"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

class Actor : public Thing
{
public:
    virtual void SetOrigin(const glm::vec3& Origin);
    virtual void SetRotation(const glm::quat& Rotation);
    virtual const glm::vec3& Origin() const;
    virtual const glm::quat& Rotation() const;

    virtual bool Initialize() { return true; }
    virtual void Destroy() {}

private:
    friend class TheatreManager;
    glm::vec3 m_Origin   = glm::vec3(0.0f);
    glm::quat m_Rotation = glm::quat();
};

#endif // ACTOR_H
