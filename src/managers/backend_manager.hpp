#ifndef BACKEND_MANAGER_H
#define BACKEND_MANAGER_H

#include "managers/manager.hpp"
#include "rendering/backends/graphics/graphics.hpp"
#include "rendering/backends/windowing/windowing.hpp"

#include <memory>

class BackendManager : public Manager
{
public:
    const char* DebugName() { return "BackendManager"; }
    bool Init();
    void Shutdown();
    ManagerEnums::TheatreReturnValue_t TheatreInit(bool IsFirstCall);
    ManagerEnums::TheatreReturnValue_t TheatreShutdown(bool IsFirstCall);

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
