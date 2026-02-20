#include "input_manager.hpp"
#include "ui_manager.hpp"
#include "application/application.hpp"
#include "events/event_queue.hpp"
#include "events/action.hpp"
#include "events/bindings.hpp"

static std::unordered_map<std::string, InputAction> sPreviousInputActions{};
static std::unordered_map<std::string, InputAction> sInputActions{};
static EventQueue sInputEventQueue{};
static InputManager sInputManager{};

std::unordered_map<uint, InputManager::InputState> InputManager::m_sInputStateBuffer{};
std::unordered_map<uint, InputManager::InputState> InputManager::m_sPreviousInputState{};
bool gPrintInputLogs{false};

InputManager* g_pInputManager{&sInputManager};

bool InputManager::Init()
{
    PRINT_PRETTY_FUNCTION;
    for(FAUTO key : Key::Keys)
        { m_sInputStateBuffer[key()] = {}; }
    m_sPreviousInputState = m_sInputStateBuffer;
    return true;
}

void InputManager::Update()
{
    m_sPreviousInputState = m_sInputStateBuffer;
    sPreviousInputActions = sInputActions;
    const std::lock_guard<std::recursive_mutex> lock{sInputEventQueue.get_mutex()};
    auto events{sInputEventQueue.get()};
    for(Shared<IEvent> event : events)
    {
        if(Shared<InputEvent> input_event{DCast<InputEvent>(event)})
        {
            g_pUIManager->Input(input_event.get());
            Application()->Input(input_event.get());
            for(auto callback : mCallbacks)
                { callback(input_event.get()); }
            if(gPrintInputLogs)
            {
                __print_verbose(true,
                    VERBOSE0,
                    "{}",
                    std::source_location::current(),
                    {"<INPUT_EVENT>",{ANSI::yellow,true,true}},
                    input_event->DebugLog());
            }
        }
    }
    sInputEventQueue.clear();
}

bool InputManager::UpdateKeyState(KeyID inKeyID, bool inCurrentState)
{
    m_sInputStateBuffer[inKeyID()].set(inCurrentState);
    for(auto& [name, action] : sInputActions)
    {
        if(action.UpdateState(inKeyID(), inCurrentState))
            { sInputEventQueue.add<InputEventAction>(action); }
    }
    return m_sInputStateBuffer[inKeyID()].just_changed();
}

EventQueue* InputManager::Queue()
{ return &sInputEventQueue; }

Error InputManager::AddAction(Farg<InputAction> inAction)
{
    if(sInputActions.contains(inAction.Name()))
        { print_error("InputAction '{}' already exists", inAction.Name()); return ERR_ALREADY_EXISTS; }
    sInputActions.emplace(inAction.Name(), inAction);
    PRINT_PRETTY_FUNCTION_EXT(" - {}", inAction.Name());
    return OK;
}

void InputManager::SetAction(Farg<InputAction> inAction)
{
    if(sInputActions.contains(inAction.Name()))
        { print_warning("InputAction '{}' already exists and will be replaced", inAction.Name()); }
    PRINT_PRETTY_FUNCTION_EXT(" - {}", inAction.Name());
    sInputActions.insert_or_assign(inAction.Name(), inAction);
}

Error InputManager::DeleteAction(Farg<std::string> inActionName)
{
    if(sInputActions.erase(inActionName))
    {
        PRINT_PRETTY_FUNCTION_EXT(" - {}", inActionName);
        return OK;
    }
    return (sInputActions.empty())
        ? ERR_EMPTY
        : ERR_NOT_FOUND;
}

void InputManager::ClearAllActions()
{ PRINT_PRETTY_FUNCTION; sInputActions.clear(); }

void InputManager::AddCallback(pInputCallback_f inCallback)
{
    const std::lock_guard<std::recursive_mutex> lock{mCallbacksMutex};
    if(auto found_it{std::find(mCallbacks.begin(), mCallbacks.end(), inCallback)};
        found_it == mCallbacks.end())
    { mCallbacks.emplace_back(inCallback); }
}

void InputManager::EraseCallback(pInputCallback_f inCallback)
{
    const std::lock_guard<std::recursive_mutex> lock{mCallbacksMutex};
    if(auto found_it{std::find(mCallbacks.begin(), mCallbacks.end(), inCallback)};
        found_it != mCallbacks.end())
    { mCallbacks.erase(found_it); }
}

bool InputManager::IsKeyDown(KeyID inKey) noexcept
{ return m_sInputStateBuffer.at(inKey()).active(); }

bool InputManager::IsKeyUp(KeyID inKey) noexcept
{ return !m_sInputStateBuffer.at(inKey()).active(); }

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

bool InputManager::IsKeyJustDown(KeyID inKey) noexcept
{ return IsKeyDown(inKey) and !m_sPreviousInputState.at(inKey()).active(); }

bool InputManager::IsKeyJustUp(KeyID inKey) noexcept
{ return IsKeyUp(inKey) and m_sPreviousInputState.at(inKey()).active(); }

bool InputManager::IsActionJustDown(const std::string& inName) noexcept
{ return IsActionDown(inName) and sPreviousInputActions.at(inName).State(); }

bool InputManager::IsActionJustUp(const std::string& inName) noexcept
{ return IsActionUp(inName) and sPreviousInputActions.at(inName).State(); }

Position2D InputManager::MousePosition() noexcept
{ return MainWindow()->GetMousePosition(); }

Position2D InputManager::LastMousePosition() noexcept
{ return MainWindow()->GetLastMousePosition(); }

Motion2D InputManager::MouseMotion() noexcept
{ return MainWindow()->GetMousePosition() - MainWindow()->GetLastMousePosition(); }
