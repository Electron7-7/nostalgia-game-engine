#ifndef ACTOR_2D_H
#define ACTOR_2D_H

#include "core/bitmask.hpp"
#include "theatre/things/thinkers/thinker.hpp"
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>

// Similar to Godot's `Node2D`
class Actor2D : public Thinker
{
public:
    virtual void Ready() override;
    virtual void SetVariables(Farg<TheatreFile::ThingData>) override;
    virtual Shared<TheatreFile::ThingData> GetVariables() const override;

    virtual Farg<glm::vec2> Position() const;
    virtual float           Rotation() const;
    virtual float           RotationDegrees() const;
    virtual Farg<glm::vec2> Scale() const;

    virtual void SetPosition(Farg<glm::vec2>);
    virtual void SetRotation(float);
    virtual void SetRotationDegrees(float);
    virtual void SetScale(Farg<glm::vec2>);


    glm::vec4 mDebugHighlight{1.0f, 0.2f, 0.9f, 0.0f}; // Off by default (alpha == 0.0f)

protected:
    glm::vec2 mPosition{};
    float     mRotationRadians{};
    float     mRotationDegrees{};
    glm::vec2 mScale{};

    BitMask mLayers{};
};
#endif // ACTOR_2D_H
