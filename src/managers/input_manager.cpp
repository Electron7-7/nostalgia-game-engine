#include "input_manager.hpp"
#include "backend_manager.hpp"
#include "common/debugging.hpp"
#include "rendering/backends/backend.hpp"
#include "input/key_handler.hpp"

// Singleton accessor
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
    PRINTDEBUG(std::string("Key pressed: ").append(1, static_cast<char>(codepoint)) + "\tKey Codepoint: " + std::to_string(codepoint));
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
    // command_buffer.AddText(new_command);
}

void InputManager::ProcessCommands()
{
    // command_buffer.BeginProcessingCommands(1);
    // while(command_buffer.DequeueNextCommand())
    // ...process console commands
    // command_buffer.EndProcessingCommands();
}
