#ifndef WINDOWING_BACKEND_H
#define WINDOWING_BACKEND_H

#include "input/fwd.hpp"

#include "backend.hpp"
#include "settings/window_info.hpp"
#include "common/safe_return.hpp"
#include "common/ids.hpp"

#include <set>
#include <glm/fwd.hpp>

enum class MouseMode
{
    Normal,
    Hidden,
    Disabled,
    Captured
};

class WindowingBackend : public _Backend
{
public:
    virtual ~WindowingBackend() = default;
    virtual bool Init() { return false; }
    virtual void Shutdown() {}
    virtual const ID& GetID() const { return BackendIDs::Invalid; }
    virtual bool InitImGui() { return false; }
    virtual void ShutdownImGui() {}
    virtual void ImGuiNewFrame() {}

    virtual void UpdateState() = 0;
    virtual SafeStatus CreateMainWindow() = 0;
    virtual void ToggleRawMouseMotion() = 0;
    virtual MouseMode ToggleMouseMode(MouseMode Secondary, MouseMode Primary = MouseMode::Normal) = 0;
    virtual void SetRawMouseMotion(bool RawMotionOn) = 0;
    virtual bool SetMouseMode(MouseMode) = 0;
    virtual MouseMode GetMouseMode() = 0;
    virtual void GetMousePosition(glm::vec2& Output) = 0;
    virtual bool UpdateBinding(InputBinding& Binding) = 0;
    virtual void SwapBuffers() = 0;
    virtual void PollEvents()  = 0;

    bool CompatibleWith(const ID& GraphicsBackend) const
    { return mCompatibleGraphicsBackends.contains(GraphicsBackend); }

protected:
    std::set<id_t> mCompatibleGraphicsBackends{};
    MouseMode mMouseMode{MouseMode::Normal};

    static WindowInfo& s_vInfo(bool GetCurrentStateInfo = true);
};

#endif // WINDOWING_BACKEND_H
