#ifndef TRANSFORM_3D_H
#define TRANSFORM_3D_H

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

class Transform3D
{
public:
    virtual const glm::vec3& cOrigin() const;
    virtual const glm::vec3& cScale() const;

    virtual glm::vec3& Origin();
    virtual glm::vec3& Scale();

    virtual glm::quat Quaternion() const;
    virtual glm::vec3 Euler(bool Degrees = false) const;
    virtual void Quaternion(const glm::quat&);
    virtual void Euler(const glm::vec3&, bool Degrees = false);

    virtual glm::vec3 Front() const;
    virtual glm::vec3 Right() const;
    virtual glm::vec3 Up() const;

protected:
    glm::vec3 m_Origin     = glm::vec3(0.0f);
    glm::quat m_Quaternion = glm::quat();
    glm::vec3 m_Euler      = glm::vec3(0.0f);
    glm::vec3 m_Scale      = glm::vec3(1.0f);
};

#endif // TRANSFORM_3D_H
