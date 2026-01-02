#ifndef CAMERA_3D_H
#define CAMERA_3D_H

#include "core/uid.hpp"
#include "things/actors/actor.hpp"
#include "rendering/render_layers.hpp"
#include "rendering/environment.hpp"

class Camera3D : public Actor
{
public:
    float mFOV{75.0f},
        mViewCutoffNear{0.01f},
        mViewCutoffFar{1000.0f};
    RenderLayers mRenderLayers{};
    Environment mEnvironment{};

    virtual void Shutdown() override;
    virtual void Ready() override;
    virtual void Tick() override;

    virtual void SetVariables(Farg<ThingData>) override;
    virtual Shared<ThingData> GetVariables() const override;

    virtual bool Current(bool isCurrent);
    virtual bool Current() const;

    virtual bool Wireframe() const override;
    virtual void Wireframe(bool) override {}

    virtual ID ViewportID() const;
    virtual Error ViewportID(ID);

    glm::mat4 ViewMatrix() const;
    glm::mat4 ProjectionMatrix() const;

protected:
    bool mInitCurrent{false};
    ID mViewportID{UID::a_MainViewport};
};

#endif // CAMERA_3D_H
