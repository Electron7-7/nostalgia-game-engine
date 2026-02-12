#ifndef EDITOR_WINDOW_H
#define EDITOR_WINDOW_H

#include <Nostalgia/Nostalgia.hpp>
#include <Nostalgia/ui/solution.hpp>

class ImGui_Editor : public UI_Solution
{
public:
    void Init()             final;
    void Shutdown()         final;
    void Update()           final;
    void Input(InputEvent*) final;
    void TheatreEntered() final;
    void TheatreExited()  final;
};

extern bool gShowDebugWindow,
    gDebugConsoleOpened,
    gTheatreInspectorActive;

extern ImGui_Editor* g_pImGuiEditor;

#endif // EDITOR_WINDOW_H
