#include "input_manager.hpp"
#include "debug.hpp"
#include "backend_manager.hpp"
#include "input/event.hpp"
#include "input/event_queue.hpp"
#include "commands/command_line.hpp"
#include "rendering/backends/backend.hpp"

using namespace ManagerEnums;

#define ASSERT_KEY(KEY)                                                       \
if(KEY > Key::HandledKeysLastIndex) return Status::InputManagerKEY_NOT_FOUND; \
if(_locked_keys.contains(KEY))      return Status::InputManagerKEY_IS_LOCKED;

static InputManager s_InputManager;
InputManager* g_pInputManager = &s_InputManager;


// PROTOTYPE FUNCTIONS
void InputManager::prototype_CustomCharacterCallback(unsigned int codepoint) const
{
    // PRINT_DEBUG("CharacterCallback - Key: '{}' | Codepoint: '{}'", static_cast<char>(codepoint), codepoint)
}
// END PROTOTYPE FUNCTIONS

bool InputManager::Init()
{
    return true;
}

void InputManager::Update()
{
    g_pBackendManager->GetWindowingBackend()->PollEvents();

    SafeStatus process_status = EventQueue::try_BeginProcessing();

    if(process_status != Status::NO_ERR && process_status != Status::EventQueueEMPTY)
    {
        PRINT_ERROR("InputManager::ProcessEvents - g_pEventSystem::BeginProcessing returned '{}'!\n", process_status.Printout())
        return;
    }

    while(EventQueue::GetCurrentQueueSize() > 0)
    {
        PRINT_DEBUG("Processing Event Queue!")

        SafeReturn<Event> next_event = EventQueue::GetNextEvent();

        if(next_event.Status() != Status::NO_ERR)
        {
            PRINT_WARNING("InputManager::ProcessEvents - g_pEventSystem::GetNextEvent returned '{}'!", next_event.Status().Printout());
            continue;
        }

        SafeStatus command_status = CommandLine::try_RunCommand(next_event.Data().GetCommand());

        if(command_status != Status::NO_ERR)
            PRINT_WARNING("InputManager::ProcessEvents - CommandLine::try_RunCommand returned '{}'\n", command_status.Printout())
    }

    EventQueue::EndProcessing();

}

SafeStatus InputManager::Press(KeyID key)
{
    ASSERT_KEY(key)

    EventQueue::try_QueueEvents(key);

    return Status::NO_ERR;
}

SafeStatus InputManager::Repeat(KeyID key)
{
    ASSERT_KEY(key)

    EventQueue::try_QueueEvents(key);

    return Status::NO_ERR;
}

SafeStatus InputManager::Release(KeyID key)
{
    ASSERT_KEY(key)

    EventQueue::try_QueueEvents(key, true);

    return Status::NO_ERR;
}
