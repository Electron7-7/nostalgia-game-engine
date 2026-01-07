#ifndef ACTOR_3D_H
#define ACTOR_3D_H

#include "thing/thinker/thinker.hpp"
#include "rendering/render_layers.hpp"
#include "theatre/transform.hpp"

class Actor3D : public Thinker, public Transform3D
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

#endif // ACTOR_3D_H
