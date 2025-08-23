#include "imgui_debugger.hpp"
#include "managers/backend_manager.hpp"
#include "managers/theatre_manager.hpp"
#include "thirdparty/DearImGui/imgui.h"
#include "settings/settings.hpp"

using namespace ImGui;

static imgui_Debugger s_Debugger;
imgui_Debugger* g_pDebugger = &s_Debugger;

bool imgui_Debugger::Init()
{
    if(is_initialized)
    { return true; }

    is_initialized = true;
    return true;
}

void imgui_Debugger::Shutdown()
{
    if(!is_initialized)
    { return; }

    is_initialized = false;
}

void imgui_Debugger::Update()
{
    Begin("Debug Window");

    if(Button("Load Test Theatre"))
    {
        if(g_pTheatreManager->try_LoadTheatre("theatres/0.HelloWorld.gt"))
        { _Manager::StartNewTheatre(); }
    }

    if(Button("Exit Test Theatre"))
    { _Manager::ShutdownTheatre(); }

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
    if(Button("Apply Changes")) { g_pBackendManager->UpdateWindowState(); }

    End();
}
