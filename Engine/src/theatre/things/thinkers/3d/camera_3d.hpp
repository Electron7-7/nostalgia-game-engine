#ifndef CAMERA_3D_H
#define CAMERA_3D_H

#include "theatre/things/thinkers/3d/actor_3d.hpp"
#include "rendering/environment.hpp"

class Camera3D : public Actor3D
{
public:
    float mFOV{75.0f},
        mViewCutoffNear{0.01f},
        mViewCutoffFar{1000.0f};
    Environment mEnvironment{};

    virtual void Ready() override;

    virtual void SetVariables(Farg<TheatreFile::ThingData>) override;
    virtual Shared<TheatreFile::ThingData> GetVariables() const override;

    virtual ID ViewportID() const;

    virtual bool Current() const;
    virtual Error SetCurrent(bool);

    virtual BitMask LayersMask() const;
    virtual void SetLayersMask(BitMask inRenderLayersMask);

    glm::mat4 ViewMatrix() const;
    glm::mat4 ProjectionMatrix() const;

protected:
    bool mInitCurrent{true};
    ID mViewportID{UID::a_RootViewport};
    BitMask mLayersMask{};

    virtual void OnAncestorRemoved(Relative) override;
    virtual void OnAncestorAdded(Relative) override;
};

#endif // CAMERA_3D_H
