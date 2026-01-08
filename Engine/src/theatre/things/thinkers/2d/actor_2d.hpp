#ifndef ACTOR_2D_H
#define ACTOR_2D_H

#include "rendering/render_layers.hpp"
#include "theatre/things/thinkers/thinker.hpp"
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>

// Similar to Godot's `Node2D`
class Actor2D : public Thinker
{
public:
    virtual void Ready() override;
    virtual void Tick() override;
    virtual void Update() override {}
    virtual void Input(InputEvent*) override {}
    virtual void SetVariables(Farg<ThingData>) override;
    virtual Shared<ThingData> GetVariables() const override;

    virtual Farg<glm::vec2> Position() const;
    virtual float           Rotation() const;
    virtual float           RotationDegrees() const;
    virtual Farg<glm::vec2> Scale() const;

    virtual void Position(Farg<glm::vec2>);
    virtual void Rotation(float);
    virtual void RotationDegrees(float);
    virtual void Scale(Farg<glm::vec2>);

    virtual bool Visible() const;
    virtual void Visible(bool isVisible);

    glm::vec4 mDebugHighlight{1.0f, 0.2f, 0.9f, 0.0f}; // Off by default (alpha == 0.0f)
    ID mDebugMeshInstanceID{};

protected:
    glm::vec2 mPosition{};
    float     mRotationRadians{};
    float     mRotationDegrees{};
    glm::vec2 mScale{};

    RenderLayers mLayers{};
    bool mVisible{true};
};
#endif // ACTOR_2D_H
