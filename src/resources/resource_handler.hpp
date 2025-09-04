#ifndef RESOURCE_HANDLER_H
#define RESOURCE_HANDLER_H

#include "resource.hpp"
#include "safe_return.hpp"

// Forward Declarations
struct TextureData;
struct Mesh;
struct Texture;
struct data_t;

struct ResourceHandler
{
public:
    static void CreateResources();
    static void DestroyResources();

    static SafeReturn<Resource*> try_GetResource(rid_t ResourceID);
    static bool try_DestroyResource(rid_t ResourceID);

private:
    static rid_t GetNewResourceID();

    static void SetVariables(size_t, rid_t, const data_t&);
};

#endif // RESOURCE_HANDLER_H
