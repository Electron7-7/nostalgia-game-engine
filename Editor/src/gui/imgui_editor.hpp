#ifndef EDITOR_WINDOW_H
#define EDITOR_WINDOW_H

#include "ui/solution.hpp"
#include "backends/imgui/imgui_implementor.hpp"

class ImGui_Editor : public UI_Solution<ImGui_Implementor>
{
    UI_SOLUTION_CONSTRUCTOR(ImGui_Editor, ImGui_Implementor)
    UI_SOLUTION_INSTANCE(ImGui_Editor)
    UI_SOLUTION_ACTIVATE(ImGui_Editor)
    UI_SOLUTION_DEACTIVATE(ImGui_Editor)
public:
    Error Init()                  final;
    void  Shutdown()              final;
    void  Update()                final;
    void  Input(FARG(InputEvent)) final;
    void  OnTheatreEntered()      final;
    void  OnTheatreExited()       final;
};

extern bool gShowDebugWindow;

#endif // EDITOR_WINDOW_H
