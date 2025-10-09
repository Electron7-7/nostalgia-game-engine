#include "input_manager.hpp"
#include "backend_manager.hpp"
#include "theatre_manager.hpp"
#include "input/event_queue.hpp"
#include "input/demo_controller.hpp"

#include <thread>

using namespace ManagerEnums;

std::vector<InputBinding> InputManager::s_Bindings = {};
InputEventCallbackFunction InputManager::m_sInputEventCallback = nullptr;
glm::vec2 InputManager::m_sMousePosition = {0.0f, 0.0f};
EventQueue InputManager::m_sInputEventQueue;
EventQueue InputManager::m_sDemoEventQueue;

static std::map<id_t, std::set<std::string>> sBindingActionsLookup = {};
static std::set<std::string> sActions = {};

static InputManager s_InputManager;
InputManager* g_pInputManager = &s_InputManager;

void InputManager::m_sHandleInputEvent(const InputEvent& event)
{
    if(m_sInputEventCallback)
        { m_sInputEventCallback(event); }
}

bool InputManager::Init()
{
    std::set<InputBinding> l_UniqueBindings = {};
    s_Bindings.reserve(BindingIDs::cBindingNames.size() - 26);
    for(const auto& [name, id] : BindingIDs::cBindingNames)
        { l_UniqueBindings.emplace(id); }
    s_Bindings.assign(l_UniqueBindings.begin(), l_UniqueBindings.end());
    return true;
}

void InputManager::Tick()
{
    EventQueue event_queue;
    mPollInputs(event_queue);
    g_pDemoController->ProcessQueue(event_queue);
    InputEvent temp_event;
    while(event_queue.GetNextEvent(temp_event))
    {
        std::thread l_InputEventCallbackThread(m_sHandleInputEvent, temp_event);
        TheatreManager::DelegateInputEvent(temp_event);
        l_InputEventCallbackThread.join();
    }
}

void InputManager::mPollInputs(EventQueue& queue)
{
    glm::vec2 last_mouse_position = m_sMousePosition;
    auto window = g_pBackendManager->Windowing();
    window->GetMousePosition(m_sMousePosition);
    window->PollEvents();
    for(auto& binding : s_Bindings)
    {
        if(binding.locked)
            { continue; }
        else if(window->GetKey(binding))
            { queue.QueueEvent({binding, sBindingActionsLookup[binding]}); }
        else if(window->GetMotion(binding, m_sMousePosition - last_mouse_position))
            { queue.QueueEvent({binding, m_sMousePosition, last_mouse_position, sBindingActionsLookup[binding]}); }
    }
}

InputBinding& InputManager::m_sGetBinding(ID id)
{
    static InputBinding l_sEmpty = InputBinding();
    auto it = std::find(s_Bindings.begin(), s_Bindings.end(), id);
    if(it != s_Bindings.end())
        { return *it; }
    return l_sEmpty;
}

bool InputManager::JustPressed(ID id)
{ return (Pressed(id) && m_sGetBinding(id).just_changed); }

bool InputManager::JustPressed(const std::string& name)
{ return JustPressed(BindingIDs::GetID(name)); }

bool InputManager::Pressed(ID id)
{ return (m_sGetBinding(id).status == InputStatus::Pressed); }

bool InputManager::Pressed(const std::string& name)
{ return Pressed(BindingIDs::GetID(name)); }

bool InputManager::Released(ID id)
{ return (m_sGetBinding(id).status == InputStatus::Released); }

bool InputManager::Released(const std::string& name)
{ return Released(BindingIDs::GetID(name)); }

bool InputManager::JustActive(ID id)
{ return JustPressed(id); }

bool InputManager::JustActive(const std::string& name)
{ return JustActive(BindingIDs::GetID(name)); }

bool InputManager::Active(ID id)
{ return Pressed(id); }

bool InputManager::Active(const std::string& name)
{ return Active(BindingIDs::GetID(name)); }

bool InputManager::Inactive(ID id)
{ return Released(id); }

bool InputManager::Inactive(const std::string& name)
{ return Inactive(BindingIDs::GetID(name)); }

InputEventCallbackFunction InputManager::SetInputEventCallback(InputEventCallbackFunction callback)
{ return (m_sInputEventCallback = callback); }

const std::set<std::string>& InputManager::GetActions(ID id)
{
    static std::set<std::string> l_sEmpty = {};
    if(!sBindingActionsLookup.contains(id))
        { return l_sEmpty; }
    return sBindingActionsLookup.at(id);
}

const std::set<std::string>& InputManager::GetActions(const std::string& name)
{ return GetActions(BindingIDs::GetID(name)); }

bool InputManager::AddAction(const std::string& action)
{ return AddAction(action, ""); }

bool InputManager::AddAction(const std::string& action, const std::string& input_name)
{ return AddAction(action, BindingIDs::GetID(input_name)); }

bool InputManager::AddAction(const std::string& action, ID input_id)
{
    if(sActions.contains(action))
    {
        print_warning("InputManager::AddAction - Action '{}' already exists", action);
        return false;
    }
    if(action.at(0) == '+' && action.size() > 1)
        { AddAction("-" + action.substr(1)); }
    sActions.emplace(action);
    if(input_id != ID::Invalid)
        { sBindingActionsLookup[input_id].emplace(action); }
    return true;
}

bool InputManager::AssignToAction(const std::string& action, const std::string& input_name)
{ return AssignToAction(action, BindingIDs::GetID(input_name)); }

bool InputManager::AssignToAction(const std::string& action, ID input_id)
{
    if(!sActions.contains(action))
    {
        print_warning("InputManager::AssignToAction - Action '{}' doesn't exist", action);
        return false;
    }
    else if(input_id == ID::Invalid)
    {
        print_warning("InputManager::AssignToAction - Invalid input ID");
        return false;
    }
    sBindingActionsLookup[input_id].emplace(action);
    return true;
}

bool InputManager::DeleteAction(const std::string& action)
{
    if(!sActions.contains(action))
        { return false; }
    for(auto& [id, actions] : sBindingActionsLookup)
        { actions.erase(action); }
    sActions.erase(action);
    return true;
}

bool InputManager::ClearActions(const std::string& input_name)
{ return ClearActions(BindingIDs::GetID(input_name)); }

bool InputManager::ClearActions(ID input_id)
{
    if(input_id == ID::Invalid)
    {
        print_warning("InputManager::ClearActions - Invalid input ID");
        return false;
    }
    sBindingActionsLookup.erase(input_id);
    return true;
}

