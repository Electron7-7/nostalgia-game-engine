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

    Farg<glm::mat4> ViewMatrix() const;

protected:
    RenderLayers mRenderLayers{};
    bool mCurrent{false};
};

class EditorCamera3D : public Camera3D
{
public:
    void SetVariables(Farg<ThingData>) final;
    Shared<ThingData> GetVariables() const final;

    void SetRenderLayers(Farg<RenderLayers>) final {}

    void SetCurrent(bool) final {}
    bool IsCurrent() const final { return true; }
};

#endif // CAMERA_3D_H
