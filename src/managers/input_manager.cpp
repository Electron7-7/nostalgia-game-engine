#include "input_manager.hpp"
#include "backend_manager.hpp"
#include "theatre_manager.hpp"
#include "input/event_queue.hpp"

#include <thread>

using namespace ManagerEnums;

static InputManager s_InputManager;
InputManager* g_pInputManager = &s_InputManager;

std::vector<InputBinding> InputManager::s_Bindings = {};
InputEventCallbackFunction InputManager::m_sInputEventCallback = nullptr;
glm::vec2 InputManager::m_sCurrentMousePosition = {0.0f, 0.0f};
glm::vec2 InputManager::m_sLastMousePosition = {0.0f, 0.0f};

static std::map<id_t, std::set<std::string>> l_sBindingActionsLookup = {};
static std::set<std::string> l_sActions = {};

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
    EventQueue::EnableEventQueue();
    return true;
}

void InputManager::Shutdown()
{
    EventQueue::EndProcessing();
    EventQueue::StopRecordingDemo();
    EventQueue::DisableEventQueue();
}

void InputManager::Update()
{
    PollInputs();
    SafeStatus process_status = EventQueue::try_BeginProcessing();
    if(process_status != Status::NO_ERR && process_status != Status::EventQueueEMPTY)
    {
        PRINT_ERROR("InputManager::ProcessEvents - g_pEventSystem::BeginProcessing returned '{}'!\n", process_status.Printout())
        return;
    }
    while(EventQueue::GetCurrentQueueSize() > 0)
    {
        auto next_event = EventQueue::GetNextEvent();
        if(next_event.Status() != Status::NO_ERR)
        {
            PRINT_WARNING("InputManager::ProcessEvents - g_pEventSystem::GetNextEvent returned '{}'!", next_event.Status().Printout());
            continue;
        }
        std::thread l_InputEventCallbackThread(m_sHandleInputEvent, next_event.Data());
        TheatreManager::DelegateInputEvent(next_event.Data());
        l_InputEventCallbackThread.join();
    }
    EventQueue::EndProcessing();
}

void InputManager::PollInputs()
{
    m_sLastMousePosition = m_sCurrentMousePosition;
    auto window = g_pBackendManager->Windowing();
    window->GetMousePosition(m_sCurrentMousePosition);
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

const BindingIterator InputManager::QueryBinding(id_t id)
{ return std::find(s_Bindings.cbegin(), s_Bindings.cend(), id); }


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

bool InputManager::AddAction(const std::string& action)
{ return AddAction(action, ""); }

bool InputManager::AddAction(const std::string& action, const std::string& input_name)
{ return AddAction(action, BindingIDs::GetID(input_name)); }

bool InputManager::AddAction(const std::string& action, ID input_id)
{
    if(sActions.contains(action))
    {
        PRINT_WARNING("InputManager::AddAction - Action '{}' already exists", action)
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
        PRINT_WARNING("InputManager::AssignToAction - Action '{}' doesn't exist", action)
        return false;
    }
    else if(input_id == ID::Invalid)
    {
        PRINT_WARNING("InputManager::AssignToAction - Invalid input ID")
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
        PRINT_WARNING("InputManager::ClearActions - Invalid input ID")
        return false;
    }
    sBindingActionsLookup.erase(input_id);
    return true;
}

