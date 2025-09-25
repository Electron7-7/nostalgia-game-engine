#ifndef MESH_INSTANCE_H
#define MESH_INSTANCE_H

#include "device.hpp"

class MeshInstance : public Device
{
public:
    virtual void SetupVariables(const data_t&);

    id_t GetMeshID() const;
    id_t GetMaterialID() const;

private:
    id_t m_MeshID     = IDs::None;
    id_t m_MaterialID = IDs::None;
};

#endif // MESH_INSTANCE_H
