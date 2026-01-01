#ifndef FONT_H
#define FONT_H

#include "things/resources/resource.hpp"

class Font : public Resource
{
public:
    virtual void SetVariables(const ThingData& data)
    {
        Resource::SetVariables(data);
    }
};

#endif // FONT_H
