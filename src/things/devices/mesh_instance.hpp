#ifndef MESH_INSTANCE_H
#define MESH_INSTANCE_H

#include "device.hpp"

class MeshInstance : public Device
{
public:
    virtual void SetupVariables(const ThingData&);

    ID GetMeshID() const;
    ID GetMaterialID() const;

private:
    ID mMeshID     = ID::None;
    ID mMaterialID = ID::None;
};

#endif // MESH_INSTANCE_H
