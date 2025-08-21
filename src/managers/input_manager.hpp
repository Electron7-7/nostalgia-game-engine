#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "manager.hpp"
#include "input/key.hpp"
#include "safe_return.hpp"

#include <set>

class InputManager : public Manager
{
public:
    bool Init();
    void Update();

    SafeStatus Press(KeyID Key);
    SafeStatus Repeat(KeyID Key);
    SafeStatus Release(KeyID Key);

    // PROTOTYPE FUNCTIONS
    void prototype_CustomCharacterCallback(unsigned int Codepoint) const;
    void prototype_TextToScreen(KeyID Key);

private:
    std::set<KeyID> _locked_keys = {};
};

extern InputManager* g_pInputManager;

#endif // INPUT_MANAGER_H
