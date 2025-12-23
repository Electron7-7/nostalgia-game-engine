#ifdef FWD_DCL
    class MeshInstance;
#elif !defined MESH_INSTANCE_H
#define MESH_INSTANCE_H

#include "device.hpp"
#include "rendering/render_layers.hpp"

class MeshInstance : public Device
{
public:
    RenderLayers mRenderLayers{};

    virtual void SetVariables(Farg<ThingData>) override;
    virtual Shared<ThingData> GetVariables() const override;

    ID GetMeshID() const;
    ID GetMaterialID() const;

protected:
    ID mMeshID{};
    ID mMaterialID{};
};

#endif // MESH_INSTANCE_H
