#ifndef CAMERA_2D_H
#define CAMERA_2D_H

#include "theatre/things/thinkers/2d/actor_2d.hpp"

class Camera2D : public Actor2D
{
public:
    virtual void Ready() override;

    virtual void SetVariables(Farg<TheatreFile::ThingData>) override;
    virtual Shared<TheatreFile::ThingData> GetVariables() const override;

    virtual ID ViewportID() const;

    virtual bool Current() const;
    virtual Error SetCurrent(bool);

    virtual BitMask LayersMask() const;
    virtual void SetLayersMask(BitMask inRenderLayersMask);

    // glm::mat4 ViewMatrix() const;
    // glm::mat4 ProjectionMatrix() const;

protected:
    glm::vec2 mZoom{1.0f, 1.0f};
    bool mInitCurrent{false};
    ID mViewportID{UID::a_Global2DViewport};
    BitMask mLayersMask{};

    virtual void OnAncestorRemoved(Relative) override;
    virtual void OnAncestorAdded(Relative) override;
};

#endif // CAMERA_2D_H
