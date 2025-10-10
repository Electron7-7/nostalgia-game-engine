#ifndef TEXTURE_H
#define TEXTURE_H

#include "resource.hpp"
#include "managers/backend_manager.hpp"

class Texture : public Resource
{
public:
    using Resource::Resource;
    virtual void SetVariables(const ThingData& data)
    {
        Resource::SetVariables(data);
        g_pBackendManager->Graphics()->BufferTexture(mFileData, mUID);
    }
};

#endif // TEXTURE_H
