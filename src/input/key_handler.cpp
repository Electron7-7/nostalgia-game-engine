#include "key_handler.hpp"
#include "managers/input_manager.hpp"

KeyHandler singleton_KeyHandler;
KeyHandler* global_KeyHandler = &singleton_KeyHandler;

bool KeyHandler::is_initialized = false;
std::map<KeyID, Key> KeyHandler::keys = {};

#define ASSERT_KEY(KeyID) if(!keys.contains(KeyID)) return STATUS_KEY_NOT_FOUND;

void KeyHandler::Init()
{
    if(is_initialized)
        return;

    // NOTE: I use unsigned int for "i" here because KeyID is also unsigned int (yay, buffer overflow!)
    for(unsigned int i = 0 ; i < CURRENT_NUMBER_OF_HANDLED_KEYS ; i++)
        keys[i] = Key(i);

    is_initialized = true;
}

bool KeyHandler::IsPressed(KeyID key_id) const
{
    if(!keys.contains(key_id))
        return false;

    return keys.at(key_id).IsPressed();
}

std::set<KeyID> KeyHandler::GetAllPressed() const
{
    std::set<KeyID> all_pressed_keys;
    for(auto& pair : keys)
        if(pair.second.IsPressed())
            all_pressed_keys.insert(pair.first);
    return all_pressed_keys;
}

// FIXME: There's got to be a better algorithm for this (or keeping parallel sets updated with the map but idk if I wanna do allat)
std::set<KeyID> KeyHandler::GetAllReleased() const
{
    std::set<KeyID> all_released_keys;
    for(auto& pair : keys)
        if(!pair.second.IsPressed())
            all_released_keys.insert(pair.first);
    return all_released_keys;
}

KeyHandler::STATUS KeyHandler::SimulateHold(KeyID key_id)
{
    ASSERT_KEY(key_id)

    keys.at(key_id).is_pressed = true;
    keys.at(key_id).is_simulating_hold = true;

    global_InputManager->prototype_CustomKeyCallback(key_id, KEY_PRESSED);
    return STATUS_SUCCESS;
}

KeyHandler::STATUS KeyHandler::SimulateHoldRelease(KeyID key_id)
{
    ASSERT_KEY(key_id)

    keys.at(key_id).is_simulating_hold = false;
    keys.at(key_id).is_pressed = false;

    global_InputManager->prototype_CustomKeyCallback(key_id, KEY_RELEASED);
    return STATUS_SUCCESS;
}

KeyHandler::STATUS KeyHandler::Press(KeyID key_id)
{
    ASSERT_KEY(key_id)

    /*
    Idea for much later:
    --------------------
        All Actors (and any other objects that can interface with inputs), should have an "Input" function that follows a standard syntax.
        Any time one of these objects is created, it's "Input" function should be added to a vector of function pointers in KeyHandler.
        Then, when KeyHandler::Press is called, it first checks if the key is currently pressed; if not, the KeyHandler will go
        through the vector of function pointers and call every single one, passing the key's ID and whether or not it's been pressed. This
        would be a rudimentary way of letting certain objects "handle inputs" without needing them to directly interface with any input
        handlers or backends.
    */

    keys.at(key_id).is_pressed = true;

    // This is useful if you want to check that the key wasn't being held when ::Press was called
    if(keys.at(key_id).is_simulating_hold)
        return STATUS_KEY_IS_SIMULATING_HOLD;

    global_InputManager->prototype_CustomKeyCallback(key_id, KEY_PRESSED);
    return STATUS_SUCCESS;
}

KeyHandler::STATUS KeyHandler::Release(KeyID key_id)
{
    ASSERT_KEY(key_id)

    if(keys.at(key_id).is_simulating_hold)
        return STATUS_KEY_IS_SIMULATING_HOLD;

    keys.at(key_id).is_pressed = true;

    global_InputManager->prototype_CustomKeyCallback(key_id, KEY_RELEASED);
    return STATUS_SUCCESS;
}
