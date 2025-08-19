#ifndef BACKEND_MANAGER_H
#define BACKEND_MANAGER_H

#include <map>

class GraphicsBackend; // Forward Declaration
class WindowingBackend; // Forward Declaration

typedef int BackendID;

class BackendManager
{
public:
    static bool Init();
    static void Shutdown();

    static GraphicsBackend* GetGraphicsBackend();
    static WindowingBackend* GetWindowingBackend();
    static BackendID GetGraphicsID();
    static BackendID GetWindowingID();

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

    static BackendID m_CurrentGraphicsID;
    static BackendID m_CurrentWindowingID;
    static BackendID m_DefaultGraphicsID;
    static BackendID m_DefaultWindowingID;

    static bool InitBackend();
};

extern BackendManager* global_BackendManager;

#endif // BACKEND_MANAGER_H
