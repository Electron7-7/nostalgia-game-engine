#ifndef INPUT_ACTION_H
#define INPUT_ACTION_H

#include "bindings.hpp"

#define INPUT_ACTION_MAX_KEYS 20

class InputAction
{
public:
    template<KeyID_t... KeyIDs> requires (sizeof...(KeyIDs) <= INPUT_ACTION_MAX_KEYS)
        InputAction(KeyIDs&&... inKeyIDs);



private:
    KeyID* mKeyIDs{nullptr};
};

#endif // INPUT_ACTION_H
