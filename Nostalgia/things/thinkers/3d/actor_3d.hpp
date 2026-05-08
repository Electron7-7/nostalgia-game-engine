#ifndef ACTOR_3D_H
#define ACTOR_3D_H

#include <Nostalgia/things/thinkers/thinker.hpp>
#include <Nostalgia/math/transform.hpp>

// Similar to Godot's `Node3D`
class Actor3D : public Thinker
{
public:
	THING_CLASS(Actor3D, Thinker)
    READY_OVERRIDE
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    virtual void SetPosition(Farg<glm::vec3>);
    virtual void SetQuaternion(Farg<glm::quat>);
    virtual void SetRotation(Farg<glm::vec3>);
    virtual void SetRotationDegrees(Farg<glm::vec3>);
    virtual void SetScale(Farg<glm::vec3>);
    virtual void SetTransform(Farg<Transform3D>);

    void SetGlobalPosition(Farg<glm::vec3>);
    void SetGlobalRotation(Farg<glm::vec3>);
    void SetGlobalRotationDegrees(Farg<glm::vec3>);
    void SetGlobalScale(Farg<glm::vec3>);
    void SetGlobalTransform(Farg<Transform3D>);

    Farg<glm::vec3> Position() const;
    Farg<glm::quat> Quaternion() const;
    Farg<glm::vec3> Rotation() const;
    Farg<glm::vec3> RotationDegrees() const;
    Farg<glm::vec3> Scale() const;
    Farg<Transform3D> Transform() const;

    Farg<glm::vec3> GlobalPosition() const;
    Farg<glm::vec3> GlobalRotation() const;
    Farg<glm::vec3> GlobalRotationDegrees() const;
    Farg<glm::vec3> GlobalScale() const;
    Farg<Transform3D> GlobalTransform() const;

protected:
    Transform3D mLocalTrans{};

    virtual void OnChildAdded(Relative) override;
    virtual void OnParentChanged(Relative, Relative) override;

private:
    mutable RMutex mTransformMutex{}; // evil mutable keyword usage bc mutex

    Transform3D mParentGlobalTrans{};
    Transform3D mGlobalTrans{mLocalTrans};

    void _set_parent_global_transform(Farg<Transform3D>);
    void _update_global_transform();
    void _update_children_global_transform();
    void _update_child_global_transform(ID);
};

#endif // ACTOR_3D_H
