#ifndef CAMERA_3D_H
#define CAMERA_3D_H

#include "core/uid.hpp"
#include "thing/thinker/actor3d/actor3d.hpp"
#include "rendering/environment.hpp"

class Camera3D : public Actor3D
{
public:
    float mFOV{75.0f},
        mViewCutoffNear{0.01f},
        mViewCutoffFar{1000.0f};
    Environment mEnvironment{};

    virtual void Shutdown() override;
    virtual void Ready() override;
    virtual void Tick() override;

    virtual void SetVariables(Farg<ThingData>) override;
    virtual Shared<ThingData> GetVariables() const override;

    virtual bool Current(bool isCurrent);
    virtual bool Current() const;

    virtual ID ViewportID() const;
    virtual Error ViewportID(ID);

    virtual RenderLayers LayersMask() const;
    virtual void LayersMask(RenderLayers);

    glm::mat4 ViewMatrix() const;
    glm::mat4 ProjectionMatrix() const;

protected:
    bool mInitCurrent{false};
    ID mViewportID{UID::a_MainViewport};
    RenderLayers mLayersMask{};
};

#endif // CAMERA_3D_H
