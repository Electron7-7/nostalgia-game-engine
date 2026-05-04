#ifndef ACTOR_2D_H
#define ACTOR_2D_H

#include <Nostalgia/things/thinkers/thinker.hpp>
#include <Nostalgia/math/transform.hpp>

// Similar to Godot's `Node2D`
class Actor2D : public Thinker
{
public:
	SET_SUPER(Thinker)
	SET_TYPEID(ThingType::Actor2D)
    READY_OVERRIDE
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    virtual void SetPosition(Farg<glm::vec2>);
    virtual void SetRotation(float);
    virtual void SetRotationDegrees(float);
    virtual void SetScale(Farg<glm::vec2>);
    virtual void SetTransform(Farg<Transform2D>);

    void SetGlobalPosition(Farg<glm::vec2>);
    void SetGlobalRotation(float);
    void SetGlobalRotationDegrees(float);
    void SetGlobalScale(Farg<glm::vec2>);
    void SetGlobalTransform(Farg<Transform2D>);

    Farg<glm::vec2> Position() const;
    float           Rotation() const;
    float           RotationDegrees() const;
    Farg<glm::vec2> Scale() const;

    Farg<glm::vec2> GlobalPosition() const;
    float GlobalRotation() const;
    float GlobalRotationDegrees() const;
    Farg<glm::vec2> GlobalScale() const;
    Farg<Transform2D> GlobalTransform() const;

protected:
    Transform2D mLocalTransform{};

    virtual void OnChildAdded(Relative) override;
    virtual void OnParentChanged(Relative, Relative) override;

private:
    mutable RMutex mTransformMutex{}; // Evil Mutable

    Transform2D mParentGlobalTransform{};
    Transform2D mGlobalTransform{mLocalTransform};

    void _set_parent_global_transform(Farg<Transform2D>);
    void _update_global_transform();
    void _update_children_global_transform();
    void _update_child_global_transform(ID);
};

#endif // ACTOR_2D_H
