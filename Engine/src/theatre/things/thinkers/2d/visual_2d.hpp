#ifndef VISUAL_2D_H
#define VISUAL_2D_H

#include "core/bitmask.hpp"
#include "theatre/things/thinkers/2d/actor_2d.hpp"

class Visual2D : public Actor2D
{
public:
    virtual void SetVariables(Farg<ThingData>) override;
    virtual Shared<ThingData> GetVariables() const override;

    virtual void SetLayers(BitMask inVisualLayers);
    virtual BitMask Layers() const;

    virtual IdSet_t Viewports() const;
    virtual void Viewports(IdSet_arg);
    virtual Error AddViewport(ID);
    virtual Error RemoveViewport(ID);
    virtual void ClearViewports();
    virtual bool IsUsingViewport(ID) const;

protected:
    BitMask mVisualLayers{};
    IdSet_t mViewportIDs{};
};

#endif // VISUAL_2D_H
