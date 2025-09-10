#include "mesh_instance.hpp"


void MeshInstance::AssignMesh(id_t mesh_id)
{ m_MeshID = mesh_id; }

id_t MeshInstance::GetMesh() const
{ return m_MeshID; }

void MeshInstance::AssignMaterial(id_t Material_id)
{ m_MaterialID = Material_id; }

id_t MeshInstance::GetMaterial() const
{ return m_MaterialID; }

bool MeshInstance::HasMaterial() const
{ return m_MaterialID != NoID; }
