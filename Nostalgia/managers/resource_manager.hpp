#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <Nostalgia/managers/manager.hpp>

class ResourceManager : public Manager
{
public:
    constexpr const char* DebugName() override { return "ResourceManager"; }
    bool Init() override;
    void Shutdown() override;
};

extern ResourceManager* g_pResourceManager;

#endif // RESOURCE_MANAGER_H
