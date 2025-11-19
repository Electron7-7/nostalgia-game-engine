#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "ui/imgui_object.hpp"

class MainWindow : public ImGui_Object
{
public:
    bool Init() final;
    void Update() final;
    void Input(const InputEvent&) final;
    void TheatreStateChanged(bool) final;
};

extern bool gShowDebugWindow;
extern MainWindow* g_pMainWindow;

#endif // MAIN_WINDOW_H
