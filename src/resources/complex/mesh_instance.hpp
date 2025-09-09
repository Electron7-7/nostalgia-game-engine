#ifndef MESH_INSTANCE_H
#define MESH_INSTANCE_H

#include "../resource.hpp"

struct MeshInstance : public Resource
{
public:
    void AssignMesh(rid_t MeshID);
    rid_t GetMesh() const;
    void AssignMaterial(rid_t MaterialID);
    rid_t GetMaterial() const;

    bool HasMaterial() const;

private:
    friend class TheatreManager;
    rid_t m_MeshID     = Resource::NoRID;
    rid_t m_MaterialID = Resource::NoRID;
};

#endif // MESH_INSTANCE_H
