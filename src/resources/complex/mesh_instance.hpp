#ifndef MESH_INSTANCE_H
#define MESH_INSTANCE_H

#include "../resource.hpp"

struct MeshInstance : public Resource
{
public:

    void AssignMesh(id_t MeshID);
    id_t GetMesh() const;
    void AssignMaterial(id_t MaterialID);
    id_t GetMaterial() const;

    bool HasMaterial() const;

private:
    friend class TheatreManager;
    id_t m_MeshID     = NoID;
    id_t m_MaterialID = NoID;
};

#endif // MESH_INSTANCE_H
