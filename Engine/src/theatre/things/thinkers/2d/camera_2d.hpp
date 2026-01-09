#ifndef CAMERA_2D_H
#define CAMERA_2D_H

#include "core/uid.hpp"
#include "core/bitmask.hpp"
#include "theatre/things/thinkers/2d/actor_2d.hpp"
#include "rendering/environment.hpp"

class Camera2D : public Actor2D
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

    virtual bool Current() const;
    virtual ID ViewportID() const;
    virtual BitMask LayersMask() const;

    virtual bool SetCurrent(bool);
    virtual Error SetViewportID(ID);
    virtual void SetLayersMask(BitMask inRenderLayersMask);

    glm::mat4 ViewMatrix() const;
    glm::mat4 ProjectionMatrix() const;

protected:
    bool mInitCurrent{false};
    ID mViewportID{UID::a_Global2DViewport};
    BitMask mLayersMask{};
};

#endif // CAMERA_2D_H
