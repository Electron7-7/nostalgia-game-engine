#include "imgui_debugger.hpp"
#include "managers/backend_manager.hpp"
#include "managers/theatre_manager.hpp"
#include "theatre/theatre_parser.hpp"
#include "settings/settings.hpp"
#include "thirdparty/DearImGui/imgui.h"
#include "thirdparty/DearImGui/imgui_stdlib.h"

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
    static std::string s_TheatreFilePath = "theatres/NostalgiaHelloWorld.nt";

    Begin("Debug Window");

#ifdef DEBUGGING
    InputInt("Parser Breakpoint Line#", &g_BreakOnLine);
    InputInt("Parser Breakpoint Column#", &g_BreakOnColumn);
#endif // DEBUGGING
    InputText("NostalgiaTheatre File Path", &s_TheatreFilePath);
    if(Button("Load Theatre"))
    {
        if(SafeStatus::PrintCheck(TheatreParser::try_LoadTheatreFromFile(s_TheatreFilePath)))
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
