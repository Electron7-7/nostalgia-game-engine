#ifndef VISUAL_3D_H
#define VISUAL_3D_H

#include "theatre/things/thinkers/3d/actor_3d.hpp"
#include "rendering/render_layers.hpp"

class Visual3D : public Actor3D
{
public:
    virtual void SetVariables(Farg<ThingData>) override;
    virtual Shared<ThingData> GetVariables() const override;

    virtual RenderLayers Layers() const;
    virtual void SetLayers(RenderLayers);

protected:
    RenderLayers mLayers{};
};


#endif // VISUAL_3D_H
