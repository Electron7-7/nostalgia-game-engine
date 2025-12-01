#ifndef IMGUI_IMPLEMENTOR_H
#define IMGUI_IMPLEMENTOR_H

#include "ui/implementor.hpp"

class ImGui_Implementor : public UI_Implementor
{
    UI_IMPLEMENTOR_CONSTRUCTOR(ImGui_Implementor)
    UI_IMPLEMENTOR_ACTIVATE(ImGui_Implementor)
    UI_IMPLEMENTOR_DEACTIVATE(ImGui_Implementor)
    UI_IMPLEMENTOR_INSTANCE(ImGui_Implementor)

public:
    void Attach() final;
    void Detach() final;
    void Begin()  final;
    void End()    final;
};

#endif // IMGUI_IMPLEMENTOR_H
