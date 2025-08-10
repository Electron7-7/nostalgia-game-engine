#include "input_manager.hpp"
#include "backend_manager.hpp"
#include "rendering/backends/backend.hpp"
#include "input/keybind.hpp"
#include "commands/command_line.hpp"
#include "input/event.hpp"
#include "input/event_queue.hpp"
#include "printing.hpp"

InputManager singleton_InputManager;
InputManager* global_InputManager = &singleton_InputManager;

#define ASSERT_KEY(KEY)                                                       \
if(KEY > Key::HandledKeysLastIndex) return Status::InputManagerKEY_NOT_FOUND; \
if(_locked_keys.contains(KEY))      return Status::InputManagerKEY_IS_LOCKED;


// PROTOTYPE FUNCTIONS
void InputManager::prototype_CustomCharacterCallback(unsigned int codepoint) const
{
    // PRINT_DEBUG("CharacterCallback - Key: '{}' | Codepoint: '{}'", static_cast<char>(codepoint), codepoint)
}
// END PROTOTYPE FUNCTIONS

bool InputManager::Init()
{
    // m_KeyBindings.SetBinding( "`", "toggleconsole" );
    // m_ButtonUpToEngine.ClearAll();
    try_AddBinding("Escape", CommandLine::cmd_ExitProgram);
    EventQueue::EnableEventQueue(); // FIXME: This should be done by the game/app, but for testing purposes I'm doing it here
    return true;
}

void InputManager::Update()
{
    global_BackendManager->GetWindowingBackend()->PollEvents();

    SafeStatus process_status = EventQueue::try_BeginProcessing();

    if(process_status != Status::NO_ERROR && process_status != Status::EventQueueEMPTY)
    {
        PRINT_ERROR("InputManager::ProcessEvents - global_EventSystem::BeginProcessing returned '{}'!\n", process_status.Printout())
        return;
    }

    while(EventQueue::GetCurrentQueueSize() > 0)
    {
        PRINTDEBUG("Processing Queue!")

        SafeReturn<Event> next_event = EventQueue::GetNextEvent();

        if(next_event.Status() != Status::NO_ERROR)
        {
            PRINT_WARNING("InputManager::ProcessEvents - global_EventSystem::GetNextEvent returned '{}'!", next_event.Status().Printout());
            continue;
        }

        SafeStatus command_status = CommandLine::try_RunCommand(next_event.Data().GetCommand());

        if(command_status != Status::NO_ERROR)
            PRINT_WARNING("InputManager::ProcessEvents - CommandLine::try_RunCommand returned '{}'\n", command_status.Printout())
    }

    EventQueue::EndProcessing();

}

SafeStatus InputManager::Press(KeyID key)
{
    ASSERT_KEY(key)

    SafeStatus test_status = EventQueue::try_QueueEvents(key);
    PRINT_DEBUG("Pressed Key '{}'", key)
    PRINT_DEBUG("EventQueue::try_QueueEvents returned '{}'", test_status.Printout())

    return Status::NO_ERROR;
}

SafeStatus InputManager::Repeat(KeyID key)
{
    ASSERT_KEY(key)

    SafeStatus test_status = EventQueue::try_QueueEvents(key);
    PRINT_DEBUG("Repeated Key '{}'", key)
    PRINT_DEBUG("EventQueue::try_QueueEvents returned '{}'", test_status.Printout())

    return Status::NO_ERROR;
}

SafeStatus InputManager::Release(KeyID key)
{
    ASSERT_KEY(key)

    SafeStatus test_status = EventQueue::try_QueueEvents(key, true);
    PRINT_DEBUG("Released Key '{}'", key)
    PRINT_DEBUG("EventQueue::try_QueueEvents returned '{}'", test_status.Printout())

    return Status::NO_ERROR;
}
