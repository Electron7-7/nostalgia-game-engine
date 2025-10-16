#ifndef MESH_INSTANCE_H
#define MESH_INSTANCE_H

#include "device.hpp"

class MeshInstance : public Device
{
public:
    virtual void SetVariables(const ThingData&);
    virtual ThingData GetVariables() const;

    ID GetMeshID() const;
    ID GetMaterialID() const;

private:
    ID mMeshID     = ID::Invalid;
    ID mMaterialID = ID::Invalid;
};

#endif // MESH_INSTANCE_H
