#ifndef MESH_INSTANCE_3D_H
#define MESH_INSTANCE_3D_H

#include "theatre/things/thinkers/3d/visual_3d.hpp"

class MeshInstance3D : public Visual3D
{
public:
    virtual void SetVariables(Farg<ThingData>) override;
    virtual Shared<ThingData> GetVariables() const override;

    virtual ID MeshID() const;
    virtual ID MaterialOverrideID() const;
    virtual bool Wireframe() const;

    virtual void SetMeshID(ID);
    virtual void SetMaterialOverrideID(ID);
    virtual void SetWireframe(bool);

protected:
    ID mMeshID{};
    ID mMaterialOverrideID{};
    bool mWireframe{false};
};


#endif // MESH_INSTANCE_3D_H
