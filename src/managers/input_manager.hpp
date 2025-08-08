#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "manager.hpp"
// #include "input/key.hpp"
#include "input/key_handler.hpp"

class InputManager : public Manager<>
{
public:
    // Inherited from _Manager
    virtual bool Init();
    virtual void Update();

    void KeyCallback(KeyID Key, KeyAction Action);

    // PROTOTYPE FUNCTIONS
    void prototype_CustomCharacterCallback(unsigned int Codepoint) const;
    void prototype_TextToScreen(KeyID Key);
};

extern InputManager* global_InputManager;

#endif // INPUT_MANAGER_H
