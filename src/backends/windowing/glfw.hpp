#ifndef GLFW_BACKEND_H
#define GLFW_BACKEND_H

#include "../windowing.hpp"
#include "frozen/map.h"

struct GLFWwindow;  // Forward Declaration
struct GLFWmonitor; // Forward Declaration

class GLFW_Backend : public WindowingBackend
{
public:
    bool Init() final;
    void Shutdown() final;
    const ID& GetID() const final
    { return BackendIDs::wGLFW; }

    bool InitImGui() final;
    void ShutdownImGui() final;
    void ImGuiNewFrame() final;

    SafeStatus CreateMainWindow() final;

    void UpdateState() final;
    MouseMode ToggleMouseMode(MouseMode, MouseMode) final;
    void ToggleRawMouseMotion() final;
    void SetRawMouseMotion(bool RawMotionOn) final;
    bool SetMouseMode(MouseMode) final;
    MouseMode GetMouseMode() final;

    void GetMousePosition(glm::vec2&) final;
    bool UpdateBinding(InputBinding&) final;
    void SwapBuffers() final;
    void PollEvents() final;

private:
    GLFWwindow* mMainWindow{nullptr};
    GLFWmonitor* mLastFullscreenedMonitor{nullptr};

    // -1 because GLFW doesn't have an ID for the `fn` key
    static constinit const frozen::map<ID, id_t, BindingIDs::KeyIDsCount + BindingIDs::MouseButtonIDsCount - 1>
        m_sInputIdToGlfw;
};

#endif // GLFW_BACKEND_H
