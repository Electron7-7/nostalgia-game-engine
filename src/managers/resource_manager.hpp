#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "manager.hpp"
#include "safe_return.hpp"
#include "resource/resource_id.hpp"

#include <string>

struct Mesh; // Forward Declaration

class ResourceManager : public Manager
{
public:
    bool Init();
    void Shutdown();
    ManagerEnums::TheatreReturnValue_t TheatreInit(bool IsFirstCall);
    ManagerEnums::TheatreReturnValue_t TheatreShutdown(bool IsFirstCall);

    ResourceID LoadGraphXMesh(Mesh* GraphXMesh, const std::string& MeshName = "UntitledGraphXMesh");
    SafeReturn<ResourceID> try_LoadOBJMeshFromMemory(const std::string& OBJData, const std::string& MeshName = "UntitledOBJMesh");
};

extern ResourceManager* g_pResourceManager;

#endif // RESOURCE_MANAGER_H
