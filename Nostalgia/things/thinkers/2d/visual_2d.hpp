#ifndef VISUAL_2D_H
#define VISUAL_2D_H

#include <Nostalgia/things/thinkers/2d/actor_2d.hpp>

class Visual2D : public Actor2D
{
public:
    SUPER(Actor2D)
    READY_OVERRIDE
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    virtual BitMask Layers() const;
    virtual void SetLayers(BitMask inVisualLayers);

    virtual ID Viewport() const;
    virtual void Viewport(ID);

    virtual bool Wireframe() const;
    virtual void SetWireframe(bool);

protected:
    BitMask mVisualLayers{};
    ID mViewportID{UID::o_RootViewport};
    bool mWireframe{false};

    virtual void OnAncestorRemoved(Relative) override;
    virtual void OnAncestorAdded(Relative) override;
};

#endif // VISUAL_2D_H
