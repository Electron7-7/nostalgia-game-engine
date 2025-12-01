#include "action.hpp"

template<KeyID_t... KeyIDs> requires (sizeof...(KeyIDs) <= INPUT_ACTION_MAX_KEYS)
    InputAction::InputAction(KeyIDs&&... inKeyIDs):
        mKeyIDs{new KeyID[sizeof...(KeyIDs)]{std::forward<KeyIDs>(inKeyIDs)...}}
    {}
