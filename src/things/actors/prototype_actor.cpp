#include "prototype_actor.hpp"

tid_t PrototypeActor::GetMeshInstance() const
{ return m_MeshInstanceID; }

bool PrototypeActor::HasMeshInstance() const
{ return (m_MeshInstanceID != Resource::NoRID); }

void PrototypeActor::SetMeshInstance(rid_t mesh_instance_id)
{ m_MeshInstanceID = mesh_instance_id; }
