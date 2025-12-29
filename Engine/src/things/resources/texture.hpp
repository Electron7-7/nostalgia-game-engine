#ifndef TEXTURE_H
#define TEXTURE_H

#include "things/resources/resource.hpp"

class Texture : public Resource
{
public:
    using Resource::Resource;

    virtual void SetVariables(Farg<ThingData>) override;
    virtual Shared<ThingData> GetVariables() const override;
};

#endif // TEXTURE_H
