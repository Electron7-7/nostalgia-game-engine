#ifndef VISUAL_3D_H
#define VISUAL_3D_H

#include "core/bitmask.hpp"
#include "theatre/things/thinkers/3d/actor_3d.hpp"

class Visual3D : public Actor3D
{
public:
    virtual void SetVariables(Farg<TheatreFile::ThingData>) override;
    virtual Shared<TheatreFile::ThingData> GetVariables() const override;

    virtual BitMask Layers() const;
    virtual void SetLayers(BitMask inVisualLayers);

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

#endif // VISUAL_3D_H
