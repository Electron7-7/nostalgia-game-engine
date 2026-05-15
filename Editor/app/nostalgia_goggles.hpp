#ifndef NOSTALGIA_GOGGLES_H
#define NOSTALGIA_GOGGLES_H

#include <Nostalgia/Nostalgia.hpp>
#include <Nostalgia/application/application.hpp>
#include <Nostalgia/ui/implementor.hpp>

// Forward Declaration
class ImGui_Editor;

class NostalgiaGoggles final : public IApplication
{
public:
    int Main() final;
    void Stop() final;
    const char* Name() final { return "Nostalgia Goggles"; }
    void Input(InputEvent*) final;
    void Event(IEvent*) final;

    const IWindow* MainWindow() const final;
    IWindow* MainWindow() final;

    inline static bool m_sEnableThingFactoryDebugMsgs{false};
    inline static uint m_sMainWindowWidth{1280}, m_sMainWindowHeight{720};

private:
    UI_Implementor* m_pUII{nullptr};
    ImGui_Editor* m_pEditor{nullptr};
    Unique<IWindow> m_pMainWindow{nullptr};
};

extern std::string gToggleFullscreen;

#endif // NOSTALGIA_GOGGLES_H
