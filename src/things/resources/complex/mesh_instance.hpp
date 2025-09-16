#ifndef MESH_INSTANCE_H
#define MESH_INSTANCE_H

#include "../resource.hpp"

class MeshInstance : public Resource
{
public:
    virtual void SetupVariables(const data_t&);

    void AssignMesh(id_t MeshID);
    id_t GetMeshID() const;

    void AssignMaterial(id_t MaterialID);
    id_t GetMaterialID() const;

private:
    id_t m_MeshID     = ID::None;
    id_t m_MaterialID = ID::None;
};

#endif // MESH_INSTANCE_H
