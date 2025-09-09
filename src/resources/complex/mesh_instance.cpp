#include "mesh_instance.hpp"

void MeshInstance::AssignMesh(rid_t mesh_id)
{ m_MeshID = mesh_id; }

rid_t MeshInstance::GetMesh() const
{ return m_MeshID; }

void MeshInstance::AssignMaterial(rid_t Material_id)
{ m_MaterialID = Material_id; }

rid_t MeshInstance::GetMaterial() const
{ return m_MaterialID; }

bool MeshInstance::HasMaterial() const
{ return m_MaterialID != Resource::NoRID; }
