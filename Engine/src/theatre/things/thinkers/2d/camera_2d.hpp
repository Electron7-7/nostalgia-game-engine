#ifndef CAMERA_2D_H
#define CAMERA_2D_H

#include "core/uid.hpp"
#include "core/bitmask.hpp"
#include "theatre/things/thinkers/2d/actor_2d.hpp"
#include "rendering/environment.hpp"

class Camera2D : public Actor2D
{
public:
    virtual void Shutdown() override;
    virtual void Ready() override;

    virtual void SetVariables(Farg<TheatreFile::ThingData>) override;
    virtual Shared<TheatreFile::ThingData> GetVariables() const override;

    virtual bool Current() const;
    virtual ID ViewportID() const;
    virtual BitMask LayersMask() const;

    virtual bool SetCurrent(bool);
    virtual Error SetViewportID(ID);
    virtual void SetLayersMask(BitMask inRenderLayersMask);

    // glm::mat4 ViewMatrix() const;
    // glm::mat4 ProjectionMatrix() const;

protected:
    glm::vec2 mZoom{1.0f, 1.0f};
    bool mInitCurrent{false};
    ID mViewportID{UID::a_Global2DViewport};
    BitMask mLayersMask{};
};

#endif // CAMERA_2D_H
