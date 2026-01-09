#ifndef VISUAL_3D_H
#define VISUAL_3D_H

#include "core/bitmask.hpp"
#include "theatre/things/thinkers/3d/actor_3d.hpp"

class Visual3D : public Actor3D
{
public:
    virtual void SetVariables(Farg<ThingData>) override;
    virtual Shared<ThingData> GetVariables() const override;

    virtual void SetLayers(BitMask inVisualLayers);
    virtual BitMask Layers() const;

protected:
    BitMask mVisualLayers{};
};

#endif // VISUAL_3D_H
