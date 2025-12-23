#ifdef FWD_DCL
    class Texture;
#elif !defined TEXTURE_H
#define TEXTURE_H

#include "resource.hpp"

class Texture : public Resource
{
public:
    using Resource::Resource;

    virtual void SetVariables(Farg<ThingData>) override;
    virtual Shared<ThingData> GetVariables() const override;
};

#endif // TEXTURE_H
