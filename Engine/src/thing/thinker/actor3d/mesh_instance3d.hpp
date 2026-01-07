#ifndef MESH_INSTANCE_H
#define MESH_INSTANCE_H

#include "thing/thinker/actor3d/actor3d.hpp"
#include "rendering/render_layers.hpp"

class __mesh_instance_t : public Actor3D
{
public:
    RenderLayers mRenderLayers{};

    virtual void SetVariables(Farg<ThingData>) override;
    virtual Shared<ThingData> GetVariables() const override;

    ID MeshID() const;
    void MeshID(ID);

    ID MaterialID() const;
    void MaterialID(ID);

protected:
    ID mMeshID{};
    ID mMaterialID{};
};

class MeshInstance3D : public __mesh_instance_t
{
public:
    virtual void SetVariables(Farg<ThingData>) override;
    virtual Shared<ThingData> GetVariables() const override;
};

// class MeshInstance2D : public __mesh_instance_t, public Transform2D
// {
// public:
    // virtual void SetVariables(Farg<ThingData>) override;
    // virtual Shared<ThingData> GetVariables() const override;
// };


#endif // MESH_INSTANCE_H
