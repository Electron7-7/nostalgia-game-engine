#include "ui_manager.hpp"
#include "ui/implementor.hpp"
#include "ui/solution.hpp"

using namespace ManagerEnums;

static UIManager s_UIManager;
UIManager* g_pUIManager{&s_UIManager};

bool UIManager::Init()
{
    PRINT_PRETTY_FUNCTION;
    UI_Implementor::AttachAll();
    UI_Implementor::InvokeMethod(&UI_Solution::Init);
    return true;
}

ManagerEnums::TheatreReturnValue_t UIManager::TheatreInit(bool first_call)
{
    if(!first_call)
        { return FINISHED; }
    UI_Implementor::InvokeMethod(&UI_Solution::TheatreEntered);
    return FINISHED;
}

void UIManager::Input(InputEvent* inInput)
{ UI_Implementor::InvokeInput(inInput); }

void UIManager::Event(IEvent* inEvent)
{ UI_Implementor::InvokeEvent(inEvent); }

void UIManager::DrawUI()
{
    UI_Implementor::InvokeMethod(&UI_Implementor::Begin);
    UI_Implementor::InvokeMethod(&UI_Solution::Update);
    UI_Implementor::InvokeMethod(&UI_Implementor::End);
}

ManagerEnums::TheatreReturnValue_t UIManager::TheatreShutdown(bool first_call)
{
    if(!first_call)
        { return FINISHED; }
    UI_Implementor::InvokeMethod(&UI_Solution::TheatreExited);
    return FINISHED;
}

void UIManager::Shutdown()
{
    PRINT_PRETTY_FUNCTION;
    UI_Implementor::InvokeMethod(&UI_Solution::Shutdown);
    UI_Implementor::DetachAll();
}
