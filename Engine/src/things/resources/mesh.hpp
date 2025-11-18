#ifndef MESH_H
#define MESH_H

#include "resource.hpp"

class Mesh : public Resource
{
public:
    using Resource::Resource;
    virtual void SetVariables(const ThingData& data)
    {
        Resource::SetVariables(data);
        // g_pBackendManager->Graphics()->BufferMesh(mFileData, mUID);
    }
};

#endif // MESH_H
