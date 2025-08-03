#include "imgui_debugger.hpp"
#include "thirdparty/DearImGui/imgui.h"

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



    End();
}
