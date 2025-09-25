#ifndef MESH_H
#define MESH_H

#include "resource.hpp"
#include "managers/backend_manager.hpp"

class Mesh : public Resource
{
public:
    using Resource::Resource;
    virtual void SetupVariables(const data_t& data)
    {
        Resource::SetupVariables(data);
        g_pBackendManager->Graphics()->BufferMesh(m_FileData, m_ID);
    }
};

#endif // MESH_H
