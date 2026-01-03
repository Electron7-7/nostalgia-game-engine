#ifndef ACTOR_H
#define ACTOR_H

#include "rendering/render_layers.hpp"
#include "things/thing.hpp"
#include "theatre/transform_3d.hpp"

class __actor_t : public Thing, public Transform3D
{
public:
    virtual void Ready() override;
    virtual void Tick() override;
    virtual void Update() override {}
    virtual void Input(InputEvent*) override {}
    virtual void SetVariables(Farg<ThingData>) override;
    virtual Shared<ThingData> GetVariables() const override;

    virtual ID DebugMeshInstance() const;
    virtual void DebugMeshInstance(ID);

    virtual bool Visible() const;
    virtual void Visible(bool isVisible);

    virtual bool Wireframe() const;
    virtual void Wireframe(bool isWireframe);

    RenderLayers mRenderLayers{};

    // Off by default (alpha == 0.0f)
    glm::vec4 mDebugHighlight{1.0f, 0.2f, 0.9f, 0.0f};

protected:
    ID mDebugMeshInstanceID{};
    bool mVisible{true};
    bool mWireframe{false};
};

class Actor : public __actor_t {};
class Actor2D : public __actor_t {};
class Actor3D : public __actor_t {};

#endif // ACTOR_H
