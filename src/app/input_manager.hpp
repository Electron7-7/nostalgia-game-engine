#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "manager.hpp"
#include <backends/backends.hpp>
#include <backends/backend_manager.hpp>

class InputManager : public Manager<>
{
public:
    // Inherited from _Manager
    virtual bool Init();
    virtual void Update();

    // Add a command into the command queue // (I think this means that inputs are processed in a queue, and as console commands)
    void AddCommand(const char* NewCommand);

    // PROTOTYPE FUNCTIONS
    void prototype_ExitOnEscapeCalled(); // NOTE: Temporary way of making sure we can exit the main loop

private:
    // Per-frame update of commands
    void ProcessCommands();
};

extern InputManager* global_InputManager;

#endif // INPUT_MANAGER_H