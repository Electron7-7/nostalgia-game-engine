#ifndef BACKEND_MANAGER_H
#define BACKEND_MANAGER_H

#include "macros.hpp"
#include <map>

FORWARD_DECLARE(class GraphicsBackend;)
FORWARD_DECLARE(class WindowingBackend;)
// This typedef keeps the code self-documenting
typedef int BackendID;
// These typedefs help make the code easier to understand in certain areas (so, more self-documentation)
typedef int GraphicsBackendID;
typedef int WindowingBackendID;

class BackendManager
{
public:
    bool Init();

    static GraphicsBackend* GetGraphicsBackend();
    static WindowingBackend* GetWindowingBackend();
    // static int RequestBackendChange(BackendID BackendID); // Note: probably not good (see source code for more detail)

    // Prototype Functions
    void prototype_RenderFrame();

private:
    static std::map<BackendID, GraphicsBackend*>  map_GraphicsBackends;
    static std::map<BackendID, WindowingBackend*> map_WindowingBackends;

    static bool is_initialized;
    static bool is_backend_initialized;
    static GraphicsBackendID current_GraphicsBackend;
    static WindowingBackendID current_WindowingBackend;

    static bool InitBackend();
};

extern BackendManager* global_BackendManager;

#endif // BACKEND_MANAGER_H
