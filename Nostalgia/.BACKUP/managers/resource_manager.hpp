#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <Nostalgia/managers/manager.hpp>
#include <Nostalgia/things/resources/resource.hpp>

class ResourceManager : public Manager
{
public:
    constexpr const char* DebugName() override { return "ResourceManager"; }
    bool Init() override;
    void Shutdown() override;
    bool AddEmbeddedResource(ID inUID);
    void RemoveEmbeddedResource(ID inUID);
    IdVec_t GetEmbeddedResources() const;
    bool IsEmbedded(ID inUID) const;
};

extern ResourceManager* g_pResourceManager;

#endif // RESOURCE_MANAGER_H
