#ifndef BACKEND_MANAGER_H
#define BACKEND_MANAGER_H

#include <map>

class GraphicsBackend; // Forward Declaration
class WindowingBackend; // Forward Declaration

// These typedefs are for self-documentation and readability
typedef int BackendID;
typedef int GraphicsBackendID;
typedef int WindowingBackendID;

class BackendManager
{
public:
    static bool Init();

    static GraphicsBackend* GetGraphicsBackend();
    static WindowingBackend* GetWindowingBackend();
    static bool InitImGui();
    static void ImGuiNewFrame();
    static void ImGuiRender();
    static void UpdateWindowState();
    // static int RequestBackendChange(BackendID BackendID); // Note: probably not good (see source code for more detail)

private:
    static std::map<BackendID, GraphicsBackend*>  map_GraphicsBackends;
    static std::map<BackendID, WindowingBackend*> map_WindowingBackends;

    static bool is_initialized;
    static bool is_backend_initialized;
    static bool is_imgui_initialized;
    static GraphicsBackendID current_GraphicsBackend;
    static WindowingBackendID current_WindowingBackend;

    static bool InitBackend();
};

extern BackendManager* global_BackendManager;

#endif // BACKEND_MANAGER_H
