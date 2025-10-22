#include "input_manager.hpp"
#include "backend_manager.hpp"
#include "theatre_manager.hpp"
#include "ui_manager.hpp"
#include "input/binding.hpp"
#include "input/demo_controller.hpp"

#include <thread>

static InputManager sInputManager;
InputManager* g_pInputManager = &sInputManager;

bool InputManager::sHandlingInputEvent{false};

void InputManager::m_sHandleInputEvent(const InputEvent& event, InputEventCallbackFunction function_ptr)
{
    if(sHandlingInputEvent || !(sHandlingInputEvent = static_cast<bool>(function_ptr)))
        { return; }
    function_ptr(event);
    sHandlingInputEvent = false;
}

bool InputManager::Init()
{
    std::vector<ID> binding_ids{};
    BindingIDs::GetAllBindingIDs(binding_ids);
    for(const ID& id : binding_ids)
        { mBindings[id] = InputBinding{id}; }
    return true;
}

void InputManager::Tick()
{
    InputEvent new_event{};
    PollInputs(new_event);
    g_pDemoController->ProcessEvent(new_event);
    std::thread l_InputEventCallbackThread(InputManager::m_sHandleInputEvent, new_event, m_pInputEventCallback);
    g_pTheatreManager->DelegateInputEvent(new_event);
    g_pUIManager->DelegateInputEvent(new_event);
    l_InputEventCallbackThread.join();
}

void InputManager::PollInputs(InputEvent& event)
{
    glm::vec2 last_mouse_position{mMousePosition};
    auto window = g_pBackendManager->Windowing();
    window->GetMousePosition(mMousePosition);
    window->PollEvents();
    event.UpdateMouseMotion(mMousePosition, last_mouse_position);
    for(auto& [id, binding] : mBindings)
    {
        if(window->UpdateBinding(binding))
            { event.add(binding); }
    }
}

InputEventCallbackFunction InputManager::SetInputEventCallback(InputEventCallbackFunction callback)
{ return (m_pInputEventCallback = callback); }

bool InputManager::BindingExists(const ID& id)
{ return mBindings.contains(id); }

bool InputManager::BindingExists(const std::string& name)
{ return BindingExists(ID{name}); }

InputBinding& InputManager::GetBinding(const ID& id)
{
    static InputBinding l_InvalidBinding{};
    if(!BindingExists(id))
        { return l_InvalidBinding; }
    return mBindings.at(id);
}

InputBinding& InputManager::GetBinding(const std::string& name)
{ return GetBinding(ID{name}); }

bool InputManager::NewAction(const std::string& action)
{ return mAllActions.add(action); }

bool InputManager::AssignAction(const std::string& action, const ID& id)
{
    NewAction(action); // Failsafe
    if(action.at(0) == '+' && action.size() > 1)
        { NewAction("-" + action.substr(1)); }
    if(InputBinding& binding = GetBinding(id); binding.IsValid())
        { return binding.mActions.add(action); }
    return false;
}

bool InputManager::AssignAction(const std::string& action, const std::string& name)
{ return AssignAction(action, ID{name}); }

bool InputManager::DeleteAction(const std::string& action)
{
    mAllActions.erase(action);
    bool return_value{false};
    for(auto& [id, binding] : mBindings)
        { return_value = (binding.mActions.erase(action)) ? true : return_value; }
    return return_value;
}

bool InputManager::ClearActions(const std::string& name)
{ return ClearActions(ID{name}); }

bool InputManager::ClearActions(const ID& id)
{
    if(!BindingExists(id))
        { return false; }
    InputActions actions = mBindings.at(id).mActions;
    mBindings.at(id).mActions.clear();
    return mAllActions.erase(actions);
}
