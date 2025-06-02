#ifndef BACKEND_MANAGER_H
#define BACKEND_MANAGER_H

#include <macros.hpp>
#include <map>

FORWARD_DECLARE(class GraphicsBackend;)
FORWARD_DECLARE(class WindowingBackend;)

class BackendManager
{
public:
    bool Init();

    static GraphicsBackend* GetGraphicsBackend();
    static WindowingBackend* GetWindowingBackend();
    static int RequestBackendChange(int BackendID); // Note: probably not good (see source code for more detail)

    // Prototype Functions
    virtual void prototype_RenderFrame();

private:
    static std::map<int, GraphicsBackend*>  map_GraphicsBackends;
    static std::map<int, WindowingBackend*> map_WindowingBackends;

    static bool is_initialized;
    static bool is_backend_initialized;
    static int current_backend;

    static bool InitBackend();
};

extern BackendManager* global_BackendManager;

#endif // BACKEND_MANAGER_H