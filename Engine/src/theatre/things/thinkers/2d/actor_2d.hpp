#ifndef ACTOR_2D_H
#define ACTOR_2D_H

#include "theatre/things/thinkers/thinker.hpp"
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>

// Similar to Godot's `Node2D`
class Actor2D : public Thinker
{
public:
    glm::vec4 mDebugHighlight{1.0f, 0.2f, 0.9f, 0.0f}; // Off by default (alpha == 0.0f)

    virtual void Ready() override;
    virtual void SetVariables(Farg<TheatreFile::ThingData>) override;
    virtual Shared<TheatreFile::ThingData> GetVariables() const override;
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

    void SetGlobalPosition(Farg<glm::vec2>);
    void SetGlobalRotation(float);
    void SetGlobalRotationDegrees(float);
    void SetGlobalScale(Farg<glm::vec2>);

protected:
    glm::vec2 mPosition{};
    float     mRotationRadians{};
    float     mRotationDegrees{};
    glm::vec2 mScale{};

    BitMask mLayers{};
};
#endif // ACTOR_2D_H
