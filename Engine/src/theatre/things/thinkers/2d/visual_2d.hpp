#ifndef VISUAL_2D_H
#define VISUAL_2D_H

#include "theatre/things/thinkers/2d/actor_2d.hpp"

class Visual2D : public Actor2D
{
public:
    virtual void Ready() override;
    virtual void SetVariables(Farg<TheatreFile::ThingData>) override;
    virtual Shared<TheatreFile::ThingData> GetVariables() const override;

    virtual BitMask Layers() const;
    virtual void SetLayers(BitMask inVisualLayers);

    virtual ID Viewport() const;
    virtual void Viewport(ID);

protected:
    BitMask mVisualLayers{};
    ID mViewportID{UID::a_RootViewport};

    virtual void OnAncestorRemoved(Relative) override;
    virtual void OnAncestorAdded(Relative) override;
};

#endif // VISUAL_2D_H
