#include "imgui_debugger.hpp"
#include "managers/backend_manager.hpp"
#include "rendering/backends/backend.hpp"
#include "thirdparty/DearImGui/imgui.h"
#include "settings/settings.hpp"

// #include <string>

using namespace ImGui;

imgui_Debugger singleton_Debugger;
imgui_Debugger* global_Debugger = &singleton_Debugger;

bool imgui_Debugger::Init()
{
    if(is_initialized)
        return true;

    is_initialized = true;
    return true;
}

void imgui_Debugger::Shutdown()
{
    if(!is_initialized)
        return;

    is_initialized = false;
}

void imgui_Debugger::Update()
{
    Begin("Debug Window");

    Text("%s", "Settings");
    Separator();
    InputInt("X Position", &Settings::Window::XPosition);
    InputInt("Y Position", &Settings::Window::YPosition);
    InputInt("Width", &Settings::Window::Width);
    InputInt("Height", &Settings::Window::Height);
    InputInt("Fullscreen Width", &Settings::Window::FullscreenWidth);
    InputInt("Fullscreen Height", &Settings::Window::FullscreenHeight);
    Separator();
    Checkbox("Fullscreen", &Settings::Window::Fullscreen);
    if(Button("Apply Changes")) { global_BackendManager->UpdateWindowState(); }

    End();
}
