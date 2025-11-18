#ifndef GLFW_BACKEND_H
#define GLFW_BACKEND_H

#include "application/window.hpp"
#include "core/id.hpp"
#include "frozen/map.h"

#include <GLFW/glfw3.h>

typedef uint GLFWInputID;

class WindowGLFW : public IWindow
{
public:
    static constexpr uint s_cBindingIDCount{BindingID::KeyIDsCount + BindingID::MouseButtonIDsCount - 1};

    WindowGLFW(const WindowProperties&);
    virtual ~WindowGLFW();

    void Update() override;

    void* GetNativeWindow() const override { return static_cast<GLFWwindow*>(m_pWindow); }
    NativeWindowType GetNativeWindowType() const override { return NATIVE_GLFW_WINDOW; }
    const std::unique_ptr<Monitor>& GetPrimaryMonitor() const override;
    uint GetFullscreenMonitorIndex() override;
    const std::unique_ptr<Monitor>& GetFullscreenMonitor() override;
    Error SetFullscreenMonitor(uint) override;


    Error SetVsync(Vsync) override;
    Error SetMouseMode(MouseMode) override;
    Error SetWindowMode(WindowMode) override;

private:
    static constinit const frozen::map<ID, GLFWInputID, s_cBindingIDCount> s_mGLFWKeyLookup;

    GLFWwindow* m_pWindow{nullptr};

    Error InitializeCallbacks() override;
    Error Init(const WindowProperties&);
    void Shutdown();

    static uint GetMonitorIndex(GLFWmonitor*);
};

#endif // GLFW_BACKEND_H
