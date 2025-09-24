#ifndef BACKEND_MANAGER_H
#define BACKEND_MANAGER_H

#include "managers/manager.hpp"
#include "rendering/backends/backend.hpp"

typedef int BackendID;

class BackendManager : public Manager
{
public:
    bool Init();
    void Shutdown();

    bool InitImGui();
    void ImGuiNewFrame();
    void ImGuiRender();

    void UpdateWindowState();

    GraphicsBackend* GetGraphicsBackend();
    WindowingBackend* GetWindowingBackend();
    BackendID GetGraphicsID();
    BackendID GetWindowingID();

private:
    bool InitBackend();

    bool m_IsImguiInitialized = false;
};

extern BackendManager* g_pBackendManager;

#endif // BACKEND_MANAGER_H
