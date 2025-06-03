#include "input_manager.hpp"
// #include "toymaker.hpp"
#include <rendering/backends/windowing/glfw.hpp>

InputManager singleton_InputManager;
InputManager* global_InputManager = &singleton_InputManager;

// PROTOTYPE FUNCTIONS

void InputManager::prototype_ExitOnEscapeCalled()
{ _Manager::Stop(); }

// END PROTOTYPE FUNCTIONS

bool InputManager::Init()
{
    // FIXME: Read keybindings from a file
    // Note: idk if that means to remove or add this functionality
    // m_KeyBindings.SetBinding( "w", "+forward" );
    // m_KeyBindings.SetBinding( "s", "+back" );
    // m_KeyBindings.SetBinding( "`", "toggleconsole" );
    // m_ButtonUpToEngine.ClearAll();
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
    // command_buffer.AddText(new_command);
}

void InputManager::ProcessCommands()
{
    // command_buffer.BeginProcessingCommands(1);
    // while(command_buffer.DequeueNextCommand())
    // ...process console commands
    // command_buffer.EndProcessingCommands();
}