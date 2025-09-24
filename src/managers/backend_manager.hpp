#ifndef BACKEND_MANAGER_H
#define BACKEND_MANAGER_H

#include "managers/manager.hpp"
#include "rendering/backends/graphics.hpp"
#include "rendering/backends/windowing.hpp"

#include <memory>

class BackendManager : public Manager
{
public:
    bool Init();
    void Shutdown();

    bool InitImGui();
    void ImGuiNewFrame();
    void ImGuiRender();

    std::shared_ptr<GraphicsBackend>  Graphics();
    std::shared_ptr<WindowingBackend> Windowing();

private:
    bool m_IsImguiInitialized = false;
};

extern BackendManager* g_pBackendManager;

#endif // BACKEND_MANAGER_H
