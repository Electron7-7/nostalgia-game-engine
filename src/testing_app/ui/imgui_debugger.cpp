#include "imgui_debugger.hpp"
#include "managers/backend_manager.hpp"
// #include "managers/theatre_manager.hpp"
#include "theatre/theatre_parser.hpp"
#include "settings/settings.hpp"
#include "thirdparty/DearImGui/imgui.h"
#include "thirdparty/DearImGui/imgui_stdlib.h"

using namespace ImGui;

static imgui_Debugger s_Debugger;
imgui_Debugger* g_pDebugger = &s_Debugger;

#ifdef DEBUGGING

// This is very inefficient and bad, but it's just for debugging and I don't want to waste time optimizing it
// (but, yes, I *do* hate the copy-pasted code)
void debug_PrintParsedTheatreData()
{
    PRINT_DEBUG("Parsed Theatre Data Printout:")

    SafeReturn<TheatreData> try_data = TheatreParser::try_GetTheatreData();
    if(!SafeStatus::PrintCheck(try_data.Status()))
    { return; }

    std::set<data_t> temp_data = try_data.Data().GetMergedData();

    for(const auto& data : temp_data)
    {
        std::print("{} {}\n", data.GetType(), data.GetName());

        const std::set<variable_t>& variables = data.m_Variables;
        for(const auto& variable : variables)
        { std::print("\t[{}] '{}': '{}' ('{}')\n", variable.m_Hash, variable.m_Name, variable.m_Value, StringifyEnum(variable.m_Type)); }

        std::print("\n");
    }
}

#endif // DEBUGGING

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

#ifdef DEBUGGING
    static std::string s_TheatreFilePath = "theatres/NostalgiaHelloWorld.nt";
    InputInt("Parser Breakpoint Line#", &g_BreakOnLine);
    InputInt("Parser Breakpoint Column#", &g_BreakOnColumn);
    InputText("NostalgiaTheatre File Path", &s_TheatreFilePath);
    if(Button("Parse NostalgiaTheatre File"))
    {
        SafeStatus::PrintCheck(TheatreParser::try_LoadTheatreFromFile(s_TheatreFilePath));
        SafeStatus::PrintCheck(TheatreParser::try_ParseTheatre());
        debug_PrintParsedTheatreData();
    }
#endif // DEBUGGING

    // if(Button("Load Test Theatre"))
    // {
    //     if(g_pTheatreManager->try_LoadTheatre("theatres/0.HelloWorld.gt"))
    //     { _Manager::StartNewTheatre(); }
    // }

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
