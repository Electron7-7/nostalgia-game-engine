#include "event_manager.hpp"
#include "events/event_queue.hpp"
#include "events/bindings.hpp"
#include "events/demo_controller.hpp"
#include "application/application.hpp"
#include "application/window.hpp"
#include "components/input_handling.hpp"

static EventManager sEventManager;
EventManager* g_pEventManager = &sEventManager;

std::array<InputEventQueue,2> EventManager::sInputQueueBuffers{InputEventQueue{},InputEventQueue{}};

bool EventManager::Init()
{
    PRINT_PRETTY_FUNCTION;
    return true;
}

void EventManager::Update()
{
    // g_pDemoController->ProcessEvent(event);
    // if(m_pInputEventCallback)
        // { m_pInputEventCallback(event); }
    // if(mDebugPrintEverySingleEventToTheConsole)
    // {
    //     __print_verbose(true,
    //         VERBOSE0,
    //         "{}",
    //         std::source_location::current(),
    //         "\x1b[32m<EVENT>",
    //         event.Log());
    // }

    // do stuff...
    sPreviousInputQueueIndex = sCurrentInputQueueIndex;
    sCurrentInputQueueIndex  = 1 - sCurrentInputQueueIndex;

}

InputEventQueue* EventManager::GetListeningInputEventQueue()
{ return &sInputQueueBuffers[sPreviousInputQueueIndex]; }

void EventManager::AddAction(FARG(InputAction) inAction)
{}

InputEventCallbackFunction EventManager::SetInputEventCallback(InputEventCallbackFunction callback)
{ return (m_pInputEventCallback = callback); }

bool EventManager::BindingExists(const ID& id)
{ return mBindings.contains(id); }

bool EventManager::BindingExists(const std::string& name)
{ return BindingExists(ID{name}); }

InputBinding& EventManager::GetBinding(const ID& id)
{
    static InputBinding l_InvalidBinding{};
    return (BindingExists(id)) ? mBindings.at(id) : l_InvalidBinding;
}

InputBinding& EventManager::GetBinding(const std::string& name)
{ return GetBinding(ID{name}); }

bool EventManager::AddAction(const std::string& action)
{ return mAllActions.add(action); }

bool EventManager::AssignAction(const std::string& action, const ID& id)
{
    AddAction(action); // Failsafe
    if(action.at(0) == '+' && action.size() > 1)
        { AddAction("-" + action.substr(1)); }
    if(InputBinding& binding = GetBinding(id); binding.IsValid())
        { return binding.mActions.add(action); }
    return false;
}

bool EventManager::AssignAction(const std::string& action, const std::string& name)
{ return AssignAction(action, ID{name}); }

bool EventManager::DeleteAction(const std::string& action)
{
    mAllActions.erase(action);
    bool return_value{false};
    for(auto& [id, binding] : mBindings)
        { return_value = (binding.mActions.erase(action)) ? true : return_value; }
    return return_value;
}

bool EventManager::ClearActions(const std::string& name)
{ return ClearActions(ID{name}); }

bool EventManager::ClearActions(const ID& id)
{
    if(!BindingExists(id))
        { return false; }
    InputActions actions = mBindings.at(id).mActions;
    mBindings.at(id).mActions.clear();
    return mAllActions.erase(actions);
}

void EventManager::ClearAllActions()
{ mAllActions.clear(); }
