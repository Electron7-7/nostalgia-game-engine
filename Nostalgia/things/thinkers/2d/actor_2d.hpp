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

    glm::vec4 mDebugHighlight{1.0f, 0.2f, 0.9f, 0.0f}; // Off by default (alpha == 0.0f)

    virtual void SetPosition(Farg<glm::vec2>);
    virtual void SetRotation(float);
    virtual void SetRotationDegrees(float);
    virtual void SetScale(Farg<glm::vec2>);

    Farg<glm::vec2> Position() const;
    float           Rotation() const;
    float           RotationDegrees() const;
    Farg<glm::vec2> Scale() const;

    glm::vec2 GlobalPosition() const;
    float GlobalRotation() const;
    float GlobalRotationDegrees() const;
    glm::vec2 GlobalScale() const;

protected:
    Transform2D mLocalTransform{};
    void _update_global_transform();

    virtual void OnChildAdded(Relative) override;

private:
    Transform2D mGlobalTransform{mLocalTransform};
    Transform2D mParentGlobalTransform{};

    void _update_global_transform(Farg<Transform2D> inParentGlobalTransform);
    void _update_children_global_transform(bool isCalledFromReady = false);
};

#endif // ACTOR_2D_H
