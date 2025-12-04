#include "event_manager.hpp"
#include "events/event_queue.hpp"
#include "events/action.hpp"
#include "events/bindings.hpp"

static EventManager sEventManager;
EventManager* g_pEventManager = &sEventManager;

std::unordered_map<KeyID, bool> EventManager::sInputStateBuffer{};
std::unordered_map<KeyID, std::vector<std::string>> EventManager::sInputActionBindingsLookup{};
std::array<InputEventQueue, 2> EventManager::sInputEventQueueBuffers{InputEventQueue{}, InputEventQueue{}};
AppEventQueue EventManager::sAppEventQueue{};

bool EventManager::Init()
{
    PRINT_PRETTY_FUNCTION;
    for(const auto& key : Key::keys)
        { sInputStateBuffer[key] = false; sInputActionBindingsLookup[key] = {}; }
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

    mPreviousInputEventQueueIndex = mCurrentInputEventQueueIndex;
    mCurrentInputEventQueueIndex  = 1 - mCurrentInputEventQueueIndex;
    sAppEventQueue.DispatchEvents();
    sAppEventQueue.clear();
    sInputEventQueueBuffers[mCurrentInputEventQueueIndex].DispatchEvents();
    sInputEventQueueBuffers[mCurrentInputEventQueueIndex].clear();
    for(auto [name, action] : mInputActions)
        { action.UpdateStatus(); }
}

bool EventManager::UpdateKeyState(FARG(KeyID) inKeyID, bool inCurrentState)
{
    bool was_just_pressed{sInputStateBuffer[inKeyID] != inCurrentState};
    sInputStateBuffer[inKeyID] = inCurrentState;
    for(const auto& name : sInputActionBindingsLookup[inKeyID])
    {
        auto& action{mInputActions.at(name)};
        action.UpdateStatus(inKeyID, inCurrentState);
        if(action.Status() || action.StatusChanged())
            { sInputEventQueueBuffers[mPreviousInputEventQueueIndex].add<InputEventAction>(action); }
    }
    return was_just_pressed;
}

InputEventQueue* EventManager::GetListeningInputEventQueue()
{ return &sInputEventQueueBuffers[mPreviousInputEventQueueIndex]; }

void EventManager::PushAppEvent(FARG(AppEvent) inAppEvent)
{ sAppEventQueue.add(inAppEvent); }

InputEventCallback EventManager::SetInputEventCallback(InputEventCallback callback)
{ return (m_pInputEventCallback = callback); }

Error EventManager::AddAction(FARG(InputAction) inAction)
{
    if(mInputActions.contains(inAction.Name()))
        { print_error("InputAction '{}' already exists", inAction.Name()); return ERR_ALREADY_EXISTS; }
    PRINT_PRETTY_FUNCTION;
    mInputActions.emplace(inAction.Name(), inAction);
    return OK;
}

void EventManager::SetAction(FARG(InputAction) inAction)
{
    if(mInputActions.contains(inAction.Name()))
        { print_warning("InputAction '{}' already exists and will be replaced", inAction.Name()); }
    PRINT_PRETTY_FUNCTION;
    mInputActions.insert_or_assign(inAction.Name(), inAction);
}

Error EventManager::DeleteAction(FARG(std::string) inActionName)
{
    if(auto found_it{mInputActions.find(inActionName)}; found_it != mInputActions.end())
        { PRINT_PRETTY_FUNCTION; mInputActions.erase(found_it); return OK; }
    return ERR_NOT_FOUND;
}

void EventManager::ClearAllActions()
{ PRINT_PRETTY_FUNCTION; mInputActions.clear(); }

