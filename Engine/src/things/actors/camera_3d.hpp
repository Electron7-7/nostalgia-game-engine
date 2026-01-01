#ifndef CAMERA_3D_H
#define CAMERA_3D_H

#include "things/actors/actor.hpp"
#include "rendering/render_layers.hpp"

class Camera3D : public Actor
{
public:
    virtual void Ready() override;
    virtual void Tick() override;

    virtual void SetVariables(Farg<ThingData>) override;
    virtual Shared<ThingData> GetVariables() const override;

    virtual void SetRenderLayers(Farg<RenderLayers> inLayers);
    virtual Farg<RenderLayers> GetRenderLayers() const;

    virtual bool Current(bool isCurrent);
    virtual bool Current() const;

    virtual bool Wireframe() const override;
    virtual void Wireframe(bool) override {}

    glm::mat4 ViewMatrix() const;

protected:
    RenderLayers mRenderLayers{};
    bool mCurrent{false};
    glm::mat4 mViewMatrix{};
};

#endif // CAMERA_3D_H
