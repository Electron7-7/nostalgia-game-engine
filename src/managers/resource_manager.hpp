#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "manager.hpp"
#include "safe_return.hpp"

#include <string>

typedef unsigned int rid_t;

struct TextureData; // Forward Declaration
struct Resource;    // Forward Declaration

class ResourceManager : public Manager
{
public:
    bool Init();
    void Shutdown();

    void ClearResources();
    bool try_RemoveResource(rid_t ResourceID);
    bool try_SlowRemoveResource(Resource* ResourcePointer); // Very slow; prefer 'rid_t'

    SafeReturn<Resource*> try_FindResource(rid_t ResourceID);

    rid_t CreateObjMesh(const std::string& OBJData, const std::string& MeshName = "UntitledOBJMesh");
    rid_t CreateTexture(const std::string& ImageFile, const std::string& TextureName = "UntitledTexture");
    rid_t CreateTexture(const TextureData& ImageData, const std::string& TextureName = "UntitledTexture");

private:
    bool m_AreResourcesLocked = false;

    void m_GenerateSpecialResources();
    void m_ClearAllResources();

    rid_t m_GetNewResourceID();

    SafeStatus m_try_CreateObjMesh(rid_t ResourceID, const std::string& ModelData, const std::string& MeshName);
    SafeStatus m_try_CreateTexture(rid_t ResourceID, const std::string& ImageFile, const std::string& TextureName);
    void m_AddTexture(rid_t ResourceID, const TextureData& ImageData, const std::string& TextureName);
};

extern ResourceManager* g_pResourceManager;

#endif // RESOURCE_MANAGER_H
