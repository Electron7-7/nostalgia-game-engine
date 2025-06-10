#ifndef KEYBOARD_HANDLER_H
#define KEYBOARD_HANDLER_H

#include <map>
#include <set>

typedef int KeyID;

struct Key
{
public:
    constexpr Key(const KeyID ID, const char Character, const bool StartPressed = false) : id(ID), character(Character), is_pressed(StartPressed), is_simulating_hold(false) {}
    constexpr Key(): Key(0, ' ', false) {}

    constexpr KeyID ID() const { return id; }
    constexpr char Character() const { return character; }
    bool IsPressed() const { return is_pressed; }

    const bool operator==(const Key& other) const { return (id == other.id); }
    const bool operator!=(const Key& other) const { return !(*this == other); }
    const bool operator==(const KeyID& other) const { return (id == other); }
    const bool operator!=(const KeyID& other) const { return !(*this == other); }

private:
    friend class KeyHandler;

    const KeyID id;
    const char character;
    bool is_pressed = false;
    bool is_simulating_hold = false;
};

inline const bool operator==(const KeyID& left, const Key& right) { return (right == left); }
inline const bool operator!=(const KeyID& left, const Key& right) { return (right != left); }

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

namespace KEY // KeyID is negative if the respective Key (in KeyHandler::keys) has no scancode
{
    constexpr KeyID LEFTSHIFT    = -8;
    constexpr KeyID RIGHTSHIFT   = -7;
    constexpr KeyID LEFTCONTROL  = -6;
    constexpr KeyID RIGHTCONTROL = -5;
    constexpr KeyID LEFTALT      = -4;
    constexpr KeyID RIGHTALT     = -3;
    constexpr KeyID LEFTSUPER    = -2;
    constexpr KeyID RIGHTSUPER   = -1;
    constexpr KeyID BACKSPACE    = 0;
    constexpr KeyID TAB          = 1;
    constexpr KeyID ENTER        = 2;
    constexpr KeyID ESC          = 3;
    constexpr KeyID SPACE        = 4;
    constexpr KeyID ZERO         = 5;
    constexpr KeyID ONE          = 6;
    constexpr KeyID TWO          = 7;
    constexpr KeyID THREE        = 8;
    constexpr KeyID FOUR         = 9;
    constexpr KeyID FIVE         = 10;
    constexpr KeyID SIX          = 11;
    constexpr KeyID SEVEN        = 12;
    constexpr KeyID EIGHT        = 13;
    constexpr KeyID NINE         = 14;
    constexpr KeyID A            = 15;
    constexpr KeyID B            = 16;
    constexpr KeyID C            = 17;
    constexpr KeyID D            = 18;
    constexpr KeyID E            = 19;
    constexpr KeyID F            = 20;
    constexpr KeyID G            = 21;
    constexpr KeyID H            = 22;
    constexpr KeyID I            = 23;
    constexpr KeyID J            = 24;
    constexpr KeyID K            = 25;
    constexpr KeyID L            = 26;
    constexpr KeyID M            = 27;
    constexpr KeyID N            = 28;
    constexpr KeyID O            = 29;
    constexpr KeyID P            = 30;
    constexpr KeyID Q            = 31;
    constexpr KeyID R            = 32;
    constexpr KeyID S            = 33;
    constexpr KeyID T            = 34;
    constexpr KeyID U            = 35;
    constexpr KeyID V            = 36;
    constexpr KeyID W            = 37;
    constexpr KeyID X            = 38;
    constexpr KeyID Y            = 39;
    constexpr KeyID Z            = 40;

    namespace UPPERCASE
    {
        constexpr KeyID A            = 41;
        constexpr KeyID B            = 42;
        constexpr KeyID C            = 43;
        constexpr KeyID D            = 44;
        constexpr KeyID E            = 45;
        constexpr KeyID F            = 46;
        constexpr KeyID G            = 47;
        constexpr KeyID H            = 48;
        constexpr KeyID I            = 49;
        constexpr KeyID J            = 50;
        constexpr KeyID K            = 51;
        constexpr KeyID L            = 52;
        constexpr KeyID M            = 53;
        constexpr KeyID N            = 54;
        constexpr KeyID O            = 55;
        constexpr KeyID P            = 56;
        constexpr KeyID Q            = 57;
        constexpr KeyID R            = 58;
        constexpr KeyID S            = 59;
        constexpr KeyID T            = 60;
        constexpr KeyID U            = 61;
        constexpr KeyID V            = 62;
        constexpr KeyID W            = 63;
        constexpr KeyID X            = 64;
        constexpr KeyID Y            = 65;
        constexpr KeyID Z            = 66;
    }
    // FIXME: Add more keys
}

inline std::map<KeyID, Key> KeyHandler::keys =
{
    // No scancodes
    { KEY::LEFTSHIFT,    Key(KEY::LEFTSHIFT,     0) },
    { KEY::RIGHTSHIFT,   Key(KEY::RIGHTSHIFT,    0) },
    { KEY::LEFTCONTROL,  Key(KEY::LEFTCONTROL,   0) },
    { KEY::RIGHTCONTROL, Key(KEY::RIGHTCONTROL,  0) },
    { KEY::LEFTALT,      Key(KEY::LEFTALT,       0) },
    { KEY::RIGHTALT,     Key(KEY::RIGHTALT,      0) },
    { KEY::LEFTSUPER,    Key(KEY::LEFTSUPER,     0) },
    { KEY::RIGHTSUPER,   Key(KEY::RIGHTSUPER,    0) },

    // Scancodes
    { KEY::BACKSPACE,    Key(KEY::BACKSPACE,    8)   },
    { KEY::TAB,          Key(KEY::TAB,          9)   },
    { KEY::ENTER,        Key(KEY::ENTER,        10)  },
    { KEY::ESC,          Key(KEY::ESC,          27)  },
    { KEY::SPACE,        Key(KEY::SPACE,        32)  },
    { KEY::ZERO,         Key(KEY::ZERO,         48)  },
    { KEY::ONE,          Key(KEY::ONE,          49)  },
    { KEY::TWO,          Key(KEY::TWO,          50)  },
    { KEY::THREE,        Key(KEY::THREE,        51)  },
    { KEY::FOUR,         Key(KEY::FOUR,         52)  },
    { KEY::FIVE,         Key(KEY::FIVE,         53)  },
    { KEY::SIX,          Key(KEY::SIX,          54)  },
    { KEY::SEVEN,        Key(KEY::SEVEN,        55)  },
    { KEY::EIGHT,        Key(KEY::EIGHT,        56)  },
    { KEY::NINE,         Key(KEY::NINE,         57)  },
    { KEY::A,            Key(KEY::A,            97)  },
    { KEY::B,            Key(KEY::B,            98)  },
    { KEY::C,            Key(KEY::C,            99)  },
    { KEY::D,            Key(KEY::D,            100) },
    { KEY::E,            Key(KEY::E,            101) },
    { KEY::F,            Key(KEY::F,            102) },
    { KEY::G,            Key(KEY::G,            103) },
    { KEY::H,            Key(KEY::H,            104) },
    { KEY::I,            Key(KEY::I,            105) },
    { KEY::J,            Key(KEY::J,            106) },
    { KEY::K,            Key(KEY::K,            107) },
    { KEY::L,            Key(KEY::L,            108) },
    { KEY::M,            Key(KEY::M,            109) },
    { KEY::N,            Key(KEY::N,            110) },
    { KEY::O,            Key(KEY::O,            111) },
    { KEY::P,            Key(KEY::P,            112) },
    { KEY::Q,            Key(KEY::Q,            113) },
    { KEY::R,            Key(KEY::R,            114) },
    { KEY::S,            Key(KEY::S,            115) },
    { KEY::T,            Key(KEY::T,            116) },
    { KEY::U,            Key(KEY::U,            117) },
    { KEY::V,            Key(KEY::V,            118) },
    { KEY::W,            Key(KEY::W,            119) },
    { KEY::X,            Key(KEY::X,            120) },
    { KEY::Y,            Key(KEY::Y,            121) },
    { KEY::Z,            Key(KEY::Z,            122) },
    { KEY::UPPERCASE::A, Key(KEY::UPPERCASE::A, 65)  },
    { KEY::UPPERCASE::B, Key(KEY::UPPERCASE::B, 66)  },
    { KEY::UPPERCASE::C, Key(KEY::UPPERCASE::C, 67)  },
    { KEY::UPPERCASE::D, Key(KEY::UPPERCASE::D, 68)  },
    { KEY::UPPERCASE::E, Key(KEY::UPPERCASE::E, 69)  },
    { KEY::UPPERCASE::F, Key(KEY::UPPERCASE::F, 70)  },
    { KEY::UPPERCASE::G, Key(KEY::UPPERCASE::G, 71)  },
    { KEY::UPPERCASE::H, Key(KEY::UPPERCASE::H, 72)  },
    { KEY::UPPERCASE::I, Key(KEY::UPPERCASE::I, 73)  },
    { KEY::UPPERCASE::J, Key(KEY::UPPERCASE::J, 74)  },
    { KEY::UPPERCASE::K, Key(KEY::UPPERCASE::K, 75)  },
    { KEY::UPPERCASE::L, Key(KEY::UPPERCASE::L, 77)  },
    { KEY::UPPERCASE::M, Key(KEY::UPPERCASE::M, 78)  },
    { KEY::UPPERCASE::N, Key(KEY::UPPERCASE::N, 79)  },
    { KEY::UPPERCASE::O, Key(KEY::UPPERCASE::O, 80)  },
    { KEY::UPPERCASE::P, Key(KEY::UPPERCASE::P, 81)  },
    { KEY::UPPERCASE::Q, Key(KEY::UPPERCASE::Q, 82)  },
    { KEY::UPPERCASE::R, Key(KEY::UPPERCASE::R, 83)  },
    { KEY::UPPERCASE::S, Key(KEY::UPPERCASE::S, 84)  },
    { KEY::UPPERCASE::T, Key(KEY::UPPERCASE::T, 85)  },
    { KEY::UPPERCASE::U, Key(KEY::UPPERCASE::U, 88)  },
    { KEY::UPPERCASE::V, Key(KEY::UPPERCASE::V, 89)  },
    { KEY::UPPERCASE::W, Key(KEY::UPPERCASE::W, 90)  },
    { KEY::UPPERCASE::X, Key(KEY::UPPERCASE::X, 91)  },
    { KEY::UPPERCASE::Y, Key(KEY::UPPERCASE::Y, 92)  },
    { KEY::UPPERCASE::Z, Key(KEY::UPPERCASE::Z, 93)  },
};

// TODO: move these somewhere nicer
typedef int KeyAction;
static constexpr KeyAction KEY_RELEASED = 0;
static constexpr KeyAction KEY_PRESSED = 1;

extern KeyHandler *global_KeyHandler;
#endif // KEYBOARD_HANDLER_H
