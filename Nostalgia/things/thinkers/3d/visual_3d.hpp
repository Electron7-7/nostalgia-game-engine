#ifndef VISUAL_3D_H
#define VISUAL_3D_H

#include <Nostalgia/things/thinkers/3d/actor_3d.hpp>

class Visual3D : public Actor3D
{
public:
	SET_SUPER(Actor3D)
	SET_TYPEID(ThingType::Visual3D)
    virtual SIG_READY override;
    virtual SIG_SET_VARIABLES override;
    virtual SIG_GET_VARIABLES override;

    virtual BitMask Layers() const;
    virtual void SetLayers(BitMask inVisualLayers);

    virtual ID Viewport() const;
    virtual void Viewport(ID);

protected:
    BitMask mVisualLayers{};
    ID mViewportID{UID::o_RootViewport};

    virtual void OnAncestorRemoved(Relative) override;
    virtual void OnAncestorAdded(Relative) override;
};

#endif // VISUAL_3D_H
