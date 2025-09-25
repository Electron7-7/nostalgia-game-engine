#ifndef FONT_H
#define FONT_H

#include "resource.hpp"

class Font : public Resource
{
public:
    using Resource::Resource;
    virtual void SetupVariables(const data_t& data)
    {
        Resource::SetupVariables(data);
        // buffer font data
    }
};

#endif // FONT_H
