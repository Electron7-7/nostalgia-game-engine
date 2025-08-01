#include "input_manager.hpp"
#include "backend_manager.hpp"
#include "rendering/backends/backend.hpp"
#include "input/key_handler.hpp"
#include "printing.hpp"
#include "console/command_line.hpp"

InputManager singleton_InputManager;
InputManager* global_InputManager = &singleton_InputManager;

// PROTOTYPE FUNCTIONS

void InputManager::prototype_CustomKeyCallback(KeyID key, KeyAction action)
{
    if(key == KEY::ESC && action == KEY_PRESSED)
        _Manager::Stop();
}

void InputManager::prototype_CustomCharacterCallback(unsigned int codepoint) const
{
    PRINT_DEBUG("Key pressed: '%c'    Key Codepoint: %3d", static_cast<char>(codepoint), codepoint);
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
    global_BackendManager->GetWindowingBackend()->prototype_PollEvents();
}

void InputManager::AddCommand(const char* new_command)
{
    // TODO: Add printouts if/when this fails to add a command to the queue.
    CommandLine::AddCommandToQueue(ConsoleCommand(new_command));
}

void InputManager::ProcessCommands()
{
    CommandLine::BeginProcessingCommands();
    while(CommandLine::DequeueNextCommand())
    {
        // ConsoleCommand current_command = CommandLine::GetCommandInQueue();
        // do something with the command(?)
        CommandLine::RunCommandInQueue();
    }
    CommandLine::EndProcessingCommands();
}
