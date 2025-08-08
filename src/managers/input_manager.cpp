#include "input_manager.hpp"
#include "backend_manager.hpp"
#include "rendering/backends/backend.hpp"
#include "input/key.hpp"
#include "events/event.hpp"
#include "input/key_handler.hpp"
#include "printing.hpp"
#include "events/event_system.hpp"

InputManager singleton_InputManager;
InputManager* global_InputManager = &singleton_InputManager;

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
    global_KeyHandler->Init();
    return true;
}

void InputManager::Update()
{
    /*
        Basically:
            1. poll the input system
            2. get event count & data
            3. get the button code & send it to the engine
            4. get the button's binding (if it has one)
            5. add the binding (which will be a command) to the command buffer
            6. run ProcessCommands() after every input event is processed
    */
    global_BackendManager->GetWindowingBackend()->PollEvents();

    /*SafeStatus process_status = global_EventSystem->try_BeginProcessing();

    if(process_status != Status::NO_ERROR && process_status != Status::EventQueueQUEUE_EMPTY)
    {
        PRINT_ERROR("InputManager::ProcessEvents - global_EventSystem::BeginProcessing returned '{}'!\n", process_status.Printout())
        return;
    }

    while(global_EventSystem->GetCurrentQueueSize() > 0)
    {
        SafeReturn<Event> next_event = global_EventSystem->GetNextEvent();

        if(next_event.GetStatus() != Status::NO_ERROR)
        {
            PRINT_WARNING("InputManager::ProcessEvents - global_EventSystem::GetNextEvent returned '{}'!", next_event.GetStatus().Printout());
            continue;
        }

        SafeStatus command_status = CommandLine::try_RunCommand(next_event.GetData().GetCommand());

        if(command_status != Status::NO_ERROR)
            PRINT_WARNING("InputManager::ProcessEvents - CommandLine::try_RunCommand returned '{}'\n", command_status.Printout())
    }

    global_EventSystem->EndProcessing();*/

}

void InputManager::KeyCallback(KeyID key, KeyAction action)
{
    /*PRINT_DEBUG("InputManager::KeyCallback - Key: '{}' | KeyAction: '{}'\n", key, (int)action);
    SafeStatus event_status = EventQueue::try_QueueEvents(key, (action == KeyAction::RELEASED));
    if(event_status != Status::NO_ERROR)
        PRINT_DEBUG("InputManager::KeyCallback - EventQueue::try_QueueEvents returned '{}'\n", event_status.Printout());*/

    // FIXME: Remove this later
    if(key == KEY::ESC && action == KeyAction::PRESSED)
        _Manager::Stop();
}
