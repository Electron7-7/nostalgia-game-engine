#ifndef CAMERA_3D_H
#define CAMERA_3D_H

#include "things/actors/actor.hpp"
#include "rendering/render_layers.hpp"

class Camera3D : public Actor
{
public:
    virtual void SetVariables(Farg<ThingData>) override;
    virtual Shared<ThingData> GetVariables() const override;

    virtual void SetRenderLayers(Farg<RenderLayers> inLayers);
    virtual Farg<RenderLayers> GetRenderLayers() const;

    virtual void SetCurrent(bool isCurrent);
    virtual bool IsCurrent() const;

    virtual void SetEnabled(bool isEnabled);
    virtual bool IsEnabled() const;

    Farg<glm::mat4> ViewMatrix() const;

protected:
    RenderLayers mRenderLayers{};
    bool mCurrent{false};
    bool mEnabled{false};
};

#endif // CAMERA_3D_H
