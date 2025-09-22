#ifndef BACKEND_MANAGER_H
#define BACKEND_MANAGER_H

#include "managers/manager.hpp"
#include "rendering/backends/backend.hpp"

#pragma message("(TODO) Implement a way to select your preferred API (e.g: Vulkan, OpenGL, etc), as I can't hot-swap the rendering API (unless I were to write my own windowing library that could, somehow, support that which is a fucking insane idea)")

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
