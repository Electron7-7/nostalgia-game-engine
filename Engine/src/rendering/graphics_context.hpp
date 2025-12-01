#ifndef GRAPHICS_CONTEXT_H
#define GRAPHICS_CONTEXT_H

#include "core/error.hpp"
#include "core/type_helpers.hpp"

class IGraphicsContext
{
public:
    virtual ~IGraphicsContext() = default;

    virtual Error Init() = 0;
    virtual void SwapBuffers() = 0;

    static Unique<IGraphicsContext> CreateContext(void* inNativeWindowPointer);
};

#endif // GRAPHICS_CONTEXT_H
