#include "input_manager.hpp"
#include "application/application.hpp"
#include "events/event_queue.hpp"
#include "events/action.hpp"
#include "events/bindings.hpp"

static InputManager sInputManager;
InputManager* g_pInputManager = &sInputManager;

std::unordered_map<std::string, InputAction> InputManager::m_sInputActions{};
std::unordered_map<uint, InputManager::InputState> InputManager::m_sInputStateBuffer{};
std::unordered_map<uint, std::vector<std::string>> InputManager::m_sInputActionBindingsLookup{};
EventQueue InputManager::m_sInputEventQueue{};

bool InputManager::Init()
{
    PRINT_PRETTY_FUNCTION;
    for(const auto& key : Key::keys)
        { m_sInputStateBuffer[key()] = {}; m_sInputActionBindingsLookup[key()] = {}; }
    return true;
}

void InputManager::Update()
{ m_sInputEventQueue.DispatchEvents(mDebugPrintEverySingleEventToTheConsole); }

bool InputManager::UpdateKeyState(FARG(KeyID) inKeyID, bool inCurrentState)
{
    m_sInputStateBuffer[inKeyID()].set(inCurrentState);
    for(const auto& name : m_sInputActionBindingsLookup[inKeyID()])
    {
        auto& action{m_sInputActions.at(name)};
        if(action.UpdateState(inKeyID(), inCurrentState))
            { m_sInputEventQueue.add<InputEventAction>(action); }
    }
    return m_sInputStateBuffer[inKeyID()].just_changed;
}

EventQueue* InputManager::Queue()
{ return &m_sInputEventQueue; }

Error InputManager::AddAction(FARG(InputAction) inAction)
{
    if(m_sInputActions.contains(inAction.Name()))
        { print_error("InputAction '{}' already exists", inAction.Name()); return ERR_ALREADY_EXISTS; }
    PRINT_PRETTY_FUNCTION;
    m_sInputActions.emplace(inAction.Name(), inAction);
    return OK;
}

void InputManager::SetAction(FARG(InputAction) inAction)
{
    if(m_sInputActions.contains(inAction.Name()))
        { print_warning("InputAction '{}' already exists and will be replaced", inAction.Name()); }
    PRINT_PRETTY_FUNCTION;
    m_sInputActions.insert_or_assign(inAction.Name(), inAction);
}

Error InputManager::DeleteAction(FARG(std::string) inActionName)
{
    if(auto found_it{m_sInputActions.find(inActionName)}; found_it != m_sInputActions.end())
        { PRINT_PRETTY_FUNCTION; m_sInputActions.erase(found_it); return OK; }
    return ERR_NOT_FOUND;
}

void InputManager::ClearAllActions()
{ PRINT_PRETTY_FUNCTION; m_sInputActions.clear(); }

bool InputManager::IsKeyDown(const KeyID& inKey) noexcept
{ return m_sInputStateBuffer.at(inKey()).active; }

bool InputManager::IsKeyUp(const KeyID& inKey) noexcept
{ return !m_sInputStateBuffer.at(inKey()).active; }

bool InputManager::IsKeyPressed(const KeyID& inKey) noexcept
{ return m_sInputStateBuffer.at(inKey()).pressed(); }

bool InputManager::IsKeyReleased(const KeyID& inKey) noexcept
{ return m_sInputStateBuffer.at(inKey()).released(); }

bool InputManager::IsActionDown(const std::string& inName) noexcept
{
    auto found_it{m_sInputActions.find(inName)};
    return (found_it != m_sInputActions.end() && found_it->second.State());
}

bool InputManager::IsActionUp(const std::string& inName) noexcept
{
    auto found_it{m_sInputActions.find(inName)};
    return (found_it == m_sInputActions.end() || !found_it->second.State());
}

bool InputManager::IsActionPressed(const std::string& inName) noexcept
{
    auto found_it{m_sInputActions.find(inName)};
    return (found_it != m_sInputActions.end() &&
        found_it->second.State() && found_it->second.StateJustChanged());
}

bool InputManager::IsActionReleased(const std::string& inName) noexcept
{
    auto found_it{m_sInputActions.find(inName)};
    return (found_it != m_sInputActions.end() &&
        !found_it->second.State() && found_it->second.StateJustChanged());
}

Position2D InputManager::MousePosition() noexcept
{ return MainWindow().GetMousePosition(); }

Position2D InputManager::LastMousePosition() noexcept
{ return MainWindow().GetLastMousePosition(); }

Motion2D InputManager::MouseMotion() noexcept
{ return MainWindow().GetLastMousePosition() - MainWindow().GetMousePosition(); }
