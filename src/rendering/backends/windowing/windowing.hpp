#ifndef WINDOWING_BACKEND_H
#define WINDOWING_BACKEND_H

#include "../backend.hpp"
#include "safe_return.hpp"

#include <set>

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
    virtual void SetFullscreen(bool FullscreenOn) = 0;
    virtual void ToggleFullscreen() = 0;

    virtual void SwapBuffers() = 0;
    virtual void PollEvents() = 0;
    virtual void UpdateState() = 0;

    bool CompatibleWith(BackendID GraphicsBackend) const { return compatible_graphics_backends.contains(GraphicsBackend); }

protected:
    std::set<BackendID> compatible_graphics_backends = {};
    bool m_IsImGuiInitialized = false;
};

#endif // WINDOWING_BACKEND_H
