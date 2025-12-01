#include "ui_manager.hpp"
#include "ui/implementor.hpp"
#include "ui/solution.hpp"
#include "core/printing.hpp"

using namespace ManagerEnums;

static UIManager s_UIManager;
UIManager* g_pUIManager = &s_UIManager;

bool UIManager::Init()
{
    PRINT_PRETTY_FUNCTION;
    IUIImplementor::AttachAll();
    return true;
}

ManagerEnums::TheatreReturnValue_t UIManager::TheatreInit(bool first_call)
{
    if(!first_call)
        { return FINISHED; }
    IUIImplementor::InvokeMethod(&IUISolution::OnTheatreEntered);
    return FINISHED;
}

void UIManager::DrawUI()
{
    IUIImplementor::InvokeMethod(&IUIImplementor::Begin);
    IUIImplementor::InvokeMethod(&IUISolution::Update);
    IUIImplementor::InvokeMethod(&IUIImplementor::End);
}

ManagerEnums::TheatreReturnValue_t UIManager::TheatreShutdown(bool first_call)
{
    if(!first_call)
        { return FINISHED; }
    IUIImplementor::InvokeMethod(&IUISolution::OnTheatreExited);
    return FINISHED;
}

void UIManager::Shutdown()
{
    PRINT_PRETTY_FUNCTION;
    IUIImplementor::DetachAll();
}
