#ifndef IMGUI_IMPLEMENTOR_H
#define IMGUI_IMPLEMENTOR_H

#include <Nostalgia/Nostalgia.hpp>
#include <Nostalgia/ui/implementor.hpp>

class ImGui_Implementor : public UI_Implementor
{
public:
    void Attach() final;
    void Detach() final;
    void Begin()  final;
    void End()    final;
};

#endif // IMGUI_IMPLEMENTOR_H
