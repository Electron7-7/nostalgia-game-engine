#ifndef BACKEND_MANAGER_H
#define BACKEND_MANAGER_H

#include <map>

// TODO: Implement a way to select your preferred API (e.g: Vulkan, OpenGL, etc), as I can't hot-swap the rendering API
// unless I were to write my own windowing library that could, somehow, support that (which is a fucking insane idea)

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

extern BackendManager* g_pBackendManager;

#endif // BACKEND_MANAGER_H
