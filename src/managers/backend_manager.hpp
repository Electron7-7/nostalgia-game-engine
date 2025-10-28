#ifndef BACKEND_MANAGER_H
#define BACKEND_MANAGER_H

#include "manager.hpp"
// I include these here instead of "backends/fwd.hpp" because you probably want to interface with a backend when you
// include the backend manager header, so why make you type allat shit out?
#include "backends/graphics.hpp"
#include "backends/windowing.hpp"

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
    bool ShutdownImGui();
    bool ImGuiNewFrame();
    bool ImGuiRender();

    std::shared_ptr<GraphicsBackend>  Graphics();
    std::shared_ptr<WindowingBackend> Windowing();

private:
    bool mImGuiInitialized{false};
};

extern BackendManager* g_pBackendManager;

#endif // BACKEND_MANAGER_H
