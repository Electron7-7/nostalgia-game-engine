#include "ui_manager.hpp"
#include "input/event.hpp"
#include "core/printing.hpp"

using namespace ManagerEnums;

static UIManager s_UIManager;
UIManager* g_pUIManager = &s_UIManager;

bool UIManager::Init()
{
    print_debug("UIManager::Init");
    // Do all non-ImGui initialization here...

    // g_pBackendManager->InitImGui();

    for(ImGui_Object* imgui_object : imgui_objects)
        { imgui_object->Init(); }

    return true;
}

ManagerEnums::TheatreReturnValue_t UIManager::TheatreInit(bool first_call)
{
    if(!first_call)
        { return FINISHED; }
    for(auto imgui_object : imgui_objects)
        { imgui_object->TheatreStateChanged(true); }
    return FINISHED;
}

void UIManager::Update()
{}

ManagerEnums::TheatreReturnValue_t UIManager::TheatreShutdown(bool first_call)
{
    if(!first_call)
        { return FINISHED; }
    for(auto imgui_object : imgui_objects)
        { imgui_object->TheatreStateChanged(false); }
    return FINISHED;
}

void UIManager::DelegateInputEvent(const InputEvent& event)
{
    for(auto imgui_object : imgui_objects)
        { imgui_object->Input(event); }
}

void UIManager::DrawUI()
{
    // Do all non-ImGui drawing here...
    DrawImGuiUI();
}

void UIManager::DrawImGuiUI()
{
    for(ImGui_Object* imgui_object : imgui_objects)
        { imgui_object->Update(); }
}

void UIManager::Shutdown()
{
    // Do all non-ImGui shutdown stuff here...
    for(ImGui_Object* imgui_object : imgui_objects)
        { imgui_object->Shutdown(); }
}

ImGui_Object* UIManager::AddImGuiObject(ImGui_Object* new_object)
{
    imgui_objects.push_back(new_object);
    imgui_objects.back()->Init();
    return imgui_objects.back();
}
