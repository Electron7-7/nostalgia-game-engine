#ifdef FWD_DCL
    class Mesh;
#elif !defined MESH_H
#define MESH_H

#include "resource.hpp"

class Mesh : public Resource
{
public:
    using Resource::Resource;

    virtual void SetVariables(const ThingData& data)
    { Resource::SetVariables(data); }
};

#endif // MESH_H
