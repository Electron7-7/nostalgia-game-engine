#include "input_manager.hpp"
#include "backend_manager.hpp"
#include "theatre_manager.hpp"
#include "input/event_queue.hpp"

#include <thread>

using namespace ManagerEnums;

static InputManager s_InputManager;
InputManager* g_pInputManager = &s_InputManager;

std::vector<binding_t> InputManager::s_Bindings = {};
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
    std::set<binding_t> l_UniqueBindings = {};
    s_Bindings.reserve(InputID::s_cBindingNames.size() - 26);
    for(const auto& [name, id] : InputID::s_cBindingNames)
        { l_UniqueBindings.emplace(id); }
    s_Bindings.assign(l_UniqueBindings.begin(), l_UniqueBindings.end());
    EventQueue::EnableEventQueue();
    return true;
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
        if(binding.Locked)
            { continue; }
        if(window->GetKey(binding))
            { EventQueue::QueueInputEvent({binding, l_sBindingActionsLookup[binding]}); }
        else if(window->GetMotion(binding, m_sCurrentMousePosition - m_sLastMousePosition))
            { EventQueue::QueueInputEvent({binding, m_sCurrentMousePosition, m_sLastMousePosition, l_sBindingActionsLookup[binding]}); }
    }
}

InputEventCallbackFunction InputManager::SetInputEventCallback(InputEventCallbackFunction callback)
{ return (m_sInputEventCallback = callback); }

bool InputManager::AddAction(const std::string& action)
{ return AddAction(action, ""); }

bool InputManager::AddAction(const std::string& action, const std::string& input_name)
{ return AddAction(action, InputID::GetID(input_name)); }

bool InputManager::AddAction(const std::string& action, id_t input_id)
{
    if(l_sActions.contains(action))
    {
        PRINT_WARNING("InputManager::AddAction - Action '{}' already exists", action)
        return false;
    }
    if(action.at(0) == '+' && action.size() > 1)
        { AddAction("-" + action.substr(1)); }
    l_sActions.emplace(action);
    if(input_id != INVALID_ID)
        { l_sBindingActionsLookup[input_id].emplace(action); }
    return true;
}

bool InputManager::AssignToAction(const std::string& action, const std::string& input_name)
{ return AssignToAction(action, InputID::GetID(input_name)); }

bool InputManager::AssignToAction(const std::string& action, id_t input_id)
{
    if(!l_sActions.contains(action))
    {
        PRINT_WARNING("InputManager::AssignToAction - Action '{}' doesn't exist", action)
        return false;
    }
    else if(input_id == INVALID_ID)
    {
        PRINT_WARNING("InputManager::AssignToAction - Invalid input ID")
        return false;
    }
    l_sBindingActionsLookup[input_id].emplace(action);
    return true;
}

bool InputManager::DeleteAction(const std::string& action)
{
    if(!l_sActions.contains(action))
        { return false; }
    for(auto& [id, actions] : l_sBindingActionsLookup)
        { actions.erase(action); }
    l_sActions.erase(action);
    return true;
}

bool InputManager::ClearActions(const std::string& input_name)
{ return ClearActions(InputID::GetID(input_name)); }

bool InputManager::ClearActions(id_t input_id)
{
    if(input_id == INVALID_ID)
    {
        PRINT_WARNING("InputManager::ClearActions - Invalid input ID")
        return false;
    }
    l_sBindingActionsLookup.erase(input_id);
    return true;
}

