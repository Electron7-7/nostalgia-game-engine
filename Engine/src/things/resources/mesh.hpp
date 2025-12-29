#ifndef MESH_H
#define MESH_H

#include "things/resources/resource.hpp"

class Mesh : public Resource
{
public:
    using Resource::Resource;

    virtual void SetVariables(const ThingData& data)
    { Resource::SetVariables(data); }
};

#endif // MESH_H
