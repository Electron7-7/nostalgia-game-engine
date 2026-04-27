#ifndef CAMERA_2D_H
#define CAMERA_2D_H

#include <Nostalgia/things/thinkers/2d/visual_2d.hpp>

class Camera2D : public Actor2D
{
public:
	SET_SUPER(Actor2D)
	SET_TYPEID(ThingType::Camera2D)
    READY_OVERRIDE
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    virtual void Draw(Shared<Visual2D>) const;

    virtual ID ViewportID() const;

    virtual bool Current() const;
    virtual Error SetCurrent(bool);

    virtual BitMask LayersMask() const;
    virtual void SetLayersMask(BitMask inRenderLayersMask);

    Farg<glm::vec2> Zoom() const;
    void SetZoom(Farg<glm::vec2>);
    void SetZoom(float);

    glm::mat4 ViewMatrix() const;
    glm::mat4 ProjectionMatrix() const;

protected:
    glm::vec2 mZoom{1.0f, 1.0f};
    bool mInitCurrent{false};
    ID mViewportID{};
    BitMask mLayersMask{};

    virtual void OnAncestorRemoved(Relative) override;
    virtual void OnAncestorAdded(Relative) override;
};

#endif // CAMERA_2D_H
