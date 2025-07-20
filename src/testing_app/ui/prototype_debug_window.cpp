#include "prototype_debug_window.hpp"
#include "thirdparty/DearImGui/imgui.h"
#include <string>

using namespace ImGui;

prototype_DebugWindow singleton_DebugWindow;
prototype_DebugWindow* global_DebugWindow = &singleton_DebugWindow;

bool prototype_DebugWindow::Init()
{
    if(is_initialized)
        return true;

    is_initialized = true;
    return true;
}

void prototype_DebugWindow::Update()
{
    Begin("Prototype Debug Window");

    Text("%s", std::string("Hello World!").c_str());
    Separator();
    Button("I don't really do anything");

    End();
}

void prototype_DebugWindow::Shutdown()
{}


