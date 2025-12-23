#include "input_manager.hpp"
#include "core/printing.hpp"
#include "application/application.hpp"
#include "events/event_queue.hpp"
#include "events/action.hpp"
#include "events/bindings.hpp"
#include "math/containers.hpp"

static std::unordered_map<std::string, InputAction> sInputActions{};
static std::unordered_map<KeyID, std::vector<std::string>> sInputActionBindingsLookup{};
static EventQueue sInputEventQueue{};
static InputManager sInputManager{};

std::unordered_map<uint, InputManager::InputState> InputManager::m_sInputStateBuffer{};

InputManager* g_pInputManager{&sInputManager};

bool InputManager::Init()
{
    PRINT_PRETTY_FUNCTION;
    for(FARG(auto) key : Key::Keys)
        { m_sInputStateBuffer[key[]] = {}; sInputActionBindingsLookup[key] = {}; }
    return true;
}

void InputManager::Update()
{ sInputEventQueue.DispatchEvents(mDebugPrintEverySingleEventToTheConsole); }

bool InputManager::UpdateKeyState(KeyID inKeyID, bool inCurrentState)
{
    m_sInputStateBuffer[inKeyID[]].set(inCurrentState);
    for(const auto& name : sInputActionBindingsLookup[inKeyID])
    {
        auto& action{sInputActions.at(name)};
        if(action.UpdateState(inKeyID[], inCurrentState))
            { sInputEventQueue.add<InputEventAction>(action); }
    }
    return m_sInputStateBuffer[inKeyID[]].just_changed;
}

EventQueue* InputManager::Queue()
{ return &sInputEventQueue; }

Error InputManager::AddAction(Farg<InputAction> inAction)
{
    if(sInputActions.contains(inAction.Name()))
        { print_error("InputAction '{}' already exists", inAction.Name()); return ERR_ALREADY_EXISTS; }
    PRINT_PRETTY_FUNCTION;
    sInputActions.emplace(inAction.Name(), inAction);
    return OK;
}

void InputManager::SetAction(Farg<InputAction> inAction)
{
    if(sInputActions.contains(inAction.Name()))
        { print_warning("InputAction '{}' already exists and will be replaced", inAction.Name()); }
    PRINT_PRETTY_FUNCTION;
    sInputActions.insert_or_assign(inAction.Name(), inAction);
}

Error InputManager::DeleteAction(Farg<std::string> inActionName)
{
    if(auto found_it{sInputActions.find(inActionName)}; found_it != sInputActions.end())
        { PRINT_PRETTY_FUNCTION; sInputActions.erase(found_it); return OK; }
    return ERR_NOT_FOUND;
}

void InputManager::ClearAllActions()
{ PRINT_PRETTY_FUNCTION; sInputActions.clear(); }

bool InputManager::IsKeyDown(KeyID inKey) noexcept
{ return m_sInputStateBuffer.at(inKey[]).active; }

bool InputManager::IsKeyUp(KeyID inKey) noexcept
{ return !m_sInputStateBuffer.at(inKey[]).active; }

bool InputManager::IsKeyPressed(KeyID inKey) noexcept
{ return m_sInputStateBuffer.at(inKey[]).pressed(); }

bool InputManager::IsKeyReleased(KeyID inKey) noexcept
{ return m_sInputStateBuffer.at(inKey[]).released(); }

bool InputManager::IsActionDown(const std::string& inName) noexcept
{
    auto found_it{sInputActions.find(inName)};
    return (found_it != sInputActions.end() && found_it->second.State());
}

bool InputManager::IsActionUp(const std::string& inName) noexcept
{
    auto found_it{sInputActions.find(inName)};
    return (found_it == sInputActions.end() || !found_it->second.State());
}

bool InputManager::IsActionPressed(const std::string& inName) noexcept
{
    auto found_it{sInputActions.find(inName)};
    return (found_it != sInputActions.end() &&
        found_it->second.State() && found_it->second.StateJustChanged());
}

bool InputManager::IsActionReleased(const std::string& inName) noexcept
{
    auto found_it{sInputActions.find(inName)};
    return (found_it != sInputActions.end() &&
        !found_it->second.State() && found_it->second.StateJustChanged());
}

Position2D InputManager::MousePosition() noexcept
{ return MainWindow()->GetMousePosition(); }

Position2D InputManager::LastMousePosition() noexcept
{ return MainWindow()->GetLastMousePosition(); }

Motion2D InputManager::MouseMotion() noexcept
{ return MainWindow()->GetLastMousePosition() - MainWindow()->GetMousePosition(); }
