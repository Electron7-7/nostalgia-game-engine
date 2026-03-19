#ifndef ACTOR_3D_H
#define ACTOR_3D_H

#include <Nostalgia/math/transform.hpp>

// Similar to Godot's `Node3D`
class Actor3D : public Thinker
{
public:
    READY_OVERRIDE
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    virtual void SetPosition(Farg<glm::vec3>);
    virtual void SetQuaternion(Farg<glm::quat>);
    virtual void SetRotation(Farg<glm::vec3>);
    virtual void SetRotationDegrees(Farg<glm::vec3>);
    virtual void SetScale(Farg<glm::vec3>);

    Farg<glm::vec3> Position() const;
    Farg<glm::quat> Quaternion() const;
    Farg<glm::vec3> Rotation() const;
    Farg<glm::vec3> RotationDegrees() const;
    Farg<glm::vec3> Scale() const;

    glm::vec3 GlobalPosition() const;
    glm::vec3 GlobalRotation() const;
    glm::vec3 GlobalRotationDegrees() const;
    glm::vec3 GlobalScale() const;

protected:
    Transform3D mLocalTransform{};

    void _update_global_transform();

    virtual void OnChildAdded(Relative) override;

private:
    Transform3D mGlobalTransform{mLocalTransform};
    Transform3D mParentGlobalTransform{};

    void _update_global_transform(Farg<Transform3D> inParentGlobalTransform);
    void _update_children_global_transform(bool isCalledFromReady = false);
};

#endif // ACTOR_3D_H
