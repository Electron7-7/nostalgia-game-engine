#ifndef FONT_H
#define FONT_H

#include "../resource.hpp"
#include "safe_return.hpp"

class Font : public Resource
{
public:
    static Font Default;

    using Resource::Resource;
    virtual void SetupVariables(const data_t&);
    virtual SafeStatus CreateResource();
};

#endif // FONT_H
