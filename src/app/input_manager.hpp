#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "manager.hpp"
#include <macros/forward_declaration.hpp>
#include <containers/double_vectors.hpp>

FORWARD_DECLARE(struct GLFWwindow;)

class InputSystem
{
public:
    bool Init();
    void Start();
    void Stop();

    DoubleVector2 GetCursorLastPosition() const;
    DoubleVector2 GetCursorCurrentPosition() const;
    bool IsButtonDown(int ScanCode) const;
    

private:
    bool is_running;

    friend class _Application;
    static void glfw_KeyCallbackFunction(GLFWwindow* Window, int Key, int ScanCode, int Action, int Mods);
    static void glfw_MouseCallbackFunction(GLFWwindow* Window, double XPosition, double YPosition);
};

class InputManager : public Manager<>
{
public:
    // Inherited from _Manager
    virtual bool Init();
    virtual void Update();

    // Add a command into the command queue
    // (I think this means that inputs are processed in a queue, and as console commands)
    void AddCommand(const char* NewCommand);

private:
    // Per-frame update of commands
    void ProcessCommands();

    // KeyBindings key_bindings;
    // CBitVec<BUTTON_CODE_LAST> m_ButtonUpToEngine; // This object is how they send button codes to the engine, like a pass-through (I think, at least)
    // CommandBuffer command_buffer;
};

extern InputManager* global_InputManager;

#endif // INPUT_MANAGER_H