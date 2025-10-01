#ifndef WINDOWING_BACKEND_H
#define WINDOWING_BACKEND_H

#include "../backend.hpp"
#include "input/fwd.hpp"
#include "safe_return.hpp"

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
    // Inherited by _Backend
    virtual ~WindowingBackend() = default;
    virtual bool Init() { return true; }
    virtual void Shutdown() { m_IsInitialized = false; m_IsImGuiInitialized = false; }
    virtual BackendID GetID() { return gBackendIDs::Invalid; }

    virtual bool InitImGui() { return true; }
    virtual void ImGuiNewFrame() {}

    virtual SafeStatus CreateMainWindow() = 0;

    virtual void ResizeWindow(int Width, int Height) = 0;
    virtual void MoveWindow(int XPosition, int YPosition) = 0;
    virtual void ToggleFullscreen() = 0;
    virtual void ToggleRawMouseMotion() = 0;
    virtual MouseMode ToggleMouseMode(MouseMode Secondary, MouseMode Primary = MouseMode::Normal) = 0;
    virtual void SetFullscreen(bool FullscreenOn) = 0;
    virtual void SetRawMouseMotion(bool RawMotionOn) = 0;
    virtual bool SetMouseMode(MouseMode) = 0;
    virtual MouseMode GetMouseMode() = 0;

    virtual void GetMousePosition(glm::vec2& Output) = 0;
    virtual bool GetKey(binding_t& Binding) = 0;
    virtual bool GetMotion(binding_t& Binding, const glm::vec2& MouseMotion) = 0;
    virtual void SwapBuffers() = 0;
    virtual void PollEvents()  = 0;
    virtual void UpdateState() = 0;

    bool CompatibleWith(BackendID GraphicsBackend) const
    { return m_CompatibleGraphicsBackends.contains(GraphicsBackend); }

protected:
    std::set<BackendID> m_CompatibleGraphicsBackends = {};
    bool m_IsImGuiInitialized = false;
    MouseMode m_MouseMode = MouseMode::Normal;
};

#endif // WINDOWING_BACKEND_H
