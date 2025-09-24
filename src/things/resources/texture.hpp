#ifndef TEXTURE_H
#define TEXTURE_H

#include "resource.hpp"

class Texture : public Resource
{
public:
    using Resource::Resource;
    virtual void SetupVariables(const data_t&);
    virtual SafeStatus CreateResource();
};

#endif // TEXTURE_H
