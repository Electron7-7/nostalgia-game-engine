#ifndef TEXTURE_H
#define TEXTURE_H

#include "resource.hpp"
#include "managers/backend_manager.hpp"

class Texture : public Resource
{
public:
    using Resource::Resource;
    virtual void SetupVariables(const data_t& data)
    {
        Resource::SetupVariables(data);
        g_pBackendManager->Graphics()->BufferTexture(m_FileData, m_ID);
    }
};

#endif // TEXTURE_H
