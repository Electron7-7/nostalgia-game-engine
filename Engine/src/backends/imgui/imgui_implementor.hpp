#ifndef IMGUI_IMPLEMENTOR_H
#define IMGUI_IMPLEMENTOR_H

#include "ui/implementor.hpp"

class ImGui_Implementor : public UI_Implementor
{
public:
    void Attach() final;
    void Detach() final;
    void Begin()  final;
    void End()    final;
};

extern ImGui_Implementor* g_pImGuiImplementor;

#endif // IMGUI_IMPLEMENTOR_H
