#ifndef KEYBOARD_HANDLER_H
#define KEYBOARD_HANDLER_H

#include <map>
#include <set>

typedef unsigned int KeyID;

// Update this number when more keys are added
#define CURRENT_NUMBER_OF_HANDLED_KEYS 50
namespace KEY
{
    constexpr KeyID ZERO         = 0;
    constexpr KeyID ONE          = 1;
    constexpr KeyID TWO          = 2;
    constexpr KeyID THREE        = 3;
    constexpr KeyID FOUR         = 4;
    constexpr KeyID FIVE         = 5;
    constexpr KeyID SIX          = 6;
    constexpr KeyID SEVEN        = 7;
    constexpr KeyID EIGHT        = 8;
    constexpr KeyID NINE         = 9;
    constexpr KeyID Q            = 10;
    constexpr KeyID W            = 11;
    constexpr KeyID E            = 12;
    constexpr KeyID R            = 13;
    constexpr KeyID T            = 14;
    constexpr KeyID Y            = 15;
    constexpr KeyID U            = 16;
    constexpr KeyID I            = 17;
    constexpr KeyID O            = 18;
    constexpr KeyID P            = 19;
    constexpr KeyID A            = 20;
    constexpr KeyID S            = 21;
    constexpr KeyID D            = 22;
    constexpr KeyID F            = 23;
    constexpr KeyID G            = 24;
    constexpr KeyID H            = 25;
    constexpr KeyID J            = 26;
    constexpr KeyID K            = 27;
    constexpr KeyID L            = 28;
    constexpr KeyID Z            = 29;
    constexpr KeyID X            = 30;
    constexpr KeyID C            = 31;
    constexpr KeyID V            = 32;
    constexpr KeyID B            = 33;
    constexpr KeyID N            = 34;
    constexpr KeyID M            = 35;
    constexpr KeyID LEFTSHIFT    = 36;
    constexpr KeyID RIGHTSHIFT   = 37;
    constexpr KeyID LEFTCONTROL  = 38;
    constexpr KeyID RIGHTCONTROL = 39;
    constexpr KeyID LEFTALT      = 40;
    constexpr KeyID RIGHTALT     = 41;
    constexpr KeyID FUNCTION     = 42;
    constexpr KeyID LEFTSUPER    = 43;
    constexpr KeyID RIGHTSUPER   = 44;
    constexpr KeyID ENTER        = 45;
    constexpr KeyID BACKSPACE    = 46;
    constexpr KeyID TAB          = 47;
    constexpr KeyID SPACE        = 48;
    constexpr KeyID ESC          = 50;
    // FIXME: Add more keys
}

struct Key
{
public:
    constexpr Key(const KeyID ID, const bool StartPressed = false) : id(ID), is_pressed(StartPressed), is_simulating_hold(false) {}
    constexpr Key(): Key(0, false) {}

    constexpr KeyID ID() const { return id; }
    bool IsPressed() const { return is_pressed; }

private:
    friend class KeyHandler;

    const KeyID id;
    bool is_pressed = false;
    bool is_simulating_hold = false;
};

class KeyHandler
{
public:
    typedef unsigned int STATUS;
    static constexpr unsigned int STATUS_SUCCESS = 0;
    static constexpr unsigned int STATUS_KEY_NOT_FOUND = 1;
    static constexpr unsigned int STATUS_KEY_IS_SIMULATING_HOLD = 2;

    static void Init();

    bool IsPressed(KeyID ID) const;
    std::set<KeyID> GetAllPressed() const;
    std::set<KeyID> GetAllReleased() const;

    KeyHandler::STATUS SimulateHold(KeyID Key);
    KeyHandler::STATUS SimulateHoldRelease(KeyID Key);

    KeyHandler::STATUS Press(KeyID Key);
    KeyHandler::STATUS Release(KeyID Key);

private:
    static std::map<KeyID, Key> keys;
    static bool is_initialized;
};

// TODO: move these somewhere nicer
typedef int KeyAction;
static constexpr KeyAction KEY_RELEASED = 0;
static constexpr KeyAction KEY_PRESSED = 1;

extern KeyHandler *global_KeyHandler;

#endif // KEYBOARD_HANDLER_H