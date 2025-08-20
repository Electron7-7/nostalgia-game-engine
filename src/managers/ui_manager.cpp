#include "ui_manager.hpp"
#include "managers/backend_manager.hpp"

using namespace ManagerEnums;

UIManager singleton_UIManager;
UIManager* global_UIManager = &singleton_UIManager;

bool UIManager::Init()
{
    if(is_initialized)
    { return true; }

    // Do all non-ImGui initialization here...

    global_BackendManager->InitImGui();

    for(ImGui_Object* imgui_object : imgui_objects)
    { imgui_object->Init(); }

    is_initialized = true;
    return true;
}

ManagerEnums::TheatreReturnValue_t UIManager::TheatreInit(bool IsFirstCall) { return ManagerEnums::TheatreReturnValue_t::FINISHED; }

void UIManager::Update()
{}

ManagerEnums::TheatreReturnValue_t UIManager::TheatreShutdown(bool IsFirstCall) { return ManagerEnums::TheatreReturnValue_t::FINISHED; }

void UIManager::DrawUI()
{
    // Do all non-ImGui drawing here...

    DrawImGuiUI();
}

void UIManager::DrawImGuiUI()
{
    global_BackendManager->ImGuiNewFrame();

    for(ImGui_Object* imgui_object : imgui_objects)
    { imgui_object->Update(); }

    global_BackendManager->ImGuiRender();
}

void UIManager::Shutdown()
{
    // Do all non-ImGui shutdown stuff here...
    for(ImGui_Object* imgui_object : imgui_objects)
    { imgui_object->Shutdown(); }
}

ImGui_Object* UIManager::AddImGuiObject(ImGui_Object* new_object)
{
    imgui_objects.insert(imgui_objects.end(), new_object);
    return imgui_objects.at(imgui_objects.size() - 1);
}
