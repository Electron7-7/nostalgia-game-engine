#ifndef INPUT_BINDINGS_H
#define INPUT_BINDINGS_H

#include "fwd.hpp"
#include "hash.hpp"
#include "frozen/map.h"

#include <string>

#define INPUT_BINDINGS_AMOUNT (InputID::End + 26 + 1) // (# of Bindings) + (2 * alphabet) + 1

struct InputID
{
    static constexpr id_t KeyZERO         = 0x00;
    static constexpr id_t KeyONE          = 0x01;
    static constexpr id_t KeyTWO          = 0x02;
    static constexpr id_t KeyTHREE        = 0x03;
    static constexpr id_t KeyFOUR         = 0x04;
    static constexpr id_t KeyFIVE         = 0x05;
    static constexpr id_t KeySIX          = 0x06;
    static constexpr id_t KeySEVEN        = 0x07;
    static constexpr id_t KeyEIGHT        = 0x08;
    static constexpr id_t KeyNINE         = 0x09;
    static constexpr id_t KeyA            = 0x0A;
    static constexpr id_t KeyB            = 0x0B;
    static constexpr id_t KeyC            = 0x0C;
    static constexpr id_t KeyD            = 0x0D;
    static constexpr id_t KeyE            = 0x0E;
    static constexpr id_t KeyF            = 0x0F;
    static constexpr id_t KeyG            = 0x10;
    static constexpr id_t KeyH            = 0x11;
    static constexpr id_t KeyI            = 0x12;
    static constexpr id_t KeyJ            = 0x13;
    static constexpr id_t KeyK            = 0x14;
    static constexpr id_t KeyL            = 0x15;
    static constexpr id_t KeyM            = 0x16;
    static constexpr id_t KeyN            = 0x17;
    static constexpr id_t KeyO            = 0x18;
    static constexpr id_t KeyP            = 0x19;
    static constexpr id_t KeyQ            = 0x1A;
    static constexpr id_t KeyR            = 0x1B;
    static constexpr id_t KeyS            = 0x1C;
    static constexpr id_t KeyT            = 0x1D;
    static constexpr id_t KeyU            = 0x1E;
    static constexpr id_t KeyV            = 0x1F;
    static constexpr id_t KeyW            = 0x20;
    static constexpr id_t KeyX            = 0x21;
    static constexpr id_t KeyY            = 0x22;
    static constexpr id_t KeyZ            = 0x23;
    static constexpr id_t KeyLEFTSHIFT    = 0x24;
    static constexpr id_t KeyRIGHTSHIFT   = 0x25;
    static constexpr id_t KeyLEFTCONTROL  = 0x26;
    static constexpr id_t KeyRIGHTCONTROL = 0x27;
    static constexpr id_t KeyLEFTALT      = 0x28;
    static constexpr id_t KeyRIGHTALT     = 0x29;
    static constexpr id_t KeyFUNCTION     = 0x2A;
    static constexpr id_t KeyLEFTSUPER    = 0x2B;
    static constexpr id_t KeyRIGHTSUPER   = 0x2C;
    static constexpr id_t KeyENTER        = 0x2D;
    static constexpr id_t KeyBACKSPACE    = 0x3E;
    static constexpr id_t KeyTAB          = 0x3F;
    static constexpr id_t KeySPACE        = 0x30;
    static constexpr id_t KeyESC          = 0x31;
    static constexpr id_t MouseLEFT       = 0x32;
    static constexpr id_t MouseRIGHT      = 0x33;
    static constexpr id_t MouseMIDDLE     = 0x34;
    static constexpr id_t MouseMotionX    = 0x35;
    static constexpr id_t MouseMotionY    = 0x36;

    static constexpr id_t Begin = KeyZERO;
    static constexpr id_t End   = MouseMotionY;

    static constexpr id_t KeysBegin = KeyZERO;
    static constexpr id_t KeysEnd   = KeyESC;
    static constexpr id_t KeysCount = KeysEnd - KeysBegin;

    static constexpr id_t MouseButtonsBegin = MouseLEFT;
    static constexpr id_t MouseButtonsEnd   = MouseMIDDLE;
    static constexpr id_t MouseButtonsCount = (MouseButtonsEnd - MouseButtonsBegin) + 1;

    static constexpr id_t MouseMotionBegin = MouseMotionX;
    static constexpr id_t MouseMotionEnd   = MouseMotionY;
    static constexpr id_t MouseMotionCount = (MouseMotionEnd - MouseMotionBegin) + 1;

    static constinit const frozen::map<hash_t, id_t, INPUT_BINDINGS_AMOUNT> s_cBindingNames;

    static constexpr bool Exists(id_t ID)
    { return (ID <= End); } // 'id_t' is unsigned and can never be below 0x00, which is our lower bound

    static constexpr bool Exists(const std::string& Name)
    { return s_cBindingNames.contains(ConstexprHash(Name)); }

    static constexpr id_t GetID(const std::string& Name)
    {
        if(!Exists(Name))
            { return INVALID_ID; }
        return s_cBindingNames.at(ConstexprHash(Name));
    }

#ifdef DEBUGGING
    static constexpr std::string debug_GetName(id_t ID)
    {
        switch(ID)
        {
        case KeyLEFTSHIFT:
            return "LShift";
        case KeyRIGHTSHIFT:
            return "RShift";
        case KeyLEFTCONTROL:
            return "LCtrl";
        case KeyRIGHTCONTROL:
            return "RCtrl";
        case KeyLEFTALT:
            return "LAlt";
        case KeyRIGHTALT:
            return "RAlt";
        case KeyFUNCTION:
            return "Fn";
        case KeyLEFTSUPER:
            return "LSuper";
        case KeyRIGHTSUPER:
            return "RSuper";
        case KeyENTER:
            return "Enter";
        case KeyBACKSPACE:
            return "Backspace";
        case KeyTAB:
            return "Tab";
        case KeySPACE:
            return "Space";
        case KeyESC:
            return "Escape";
        case MouseLEFT:
            return "LeftMouse";
        case MouseRIGHT:
            return "RightMouse";
        case MouseMIDDLE:
            return "MiddleMouse";
        case MouseMotionX:
            return "MouseMotionX";
        case MouseMotionY:
            return "MouseMotionY";
        case KeyZERO:
            return "0";
        case KeyONE:
            return "1";
        case KeyTWO:
            return "2";
        case KeyTHREE:
            return "3";
        case KeyFOUR:
            return "4";
        case KeyFIVE:
            return "5";
        case KeySIX:
            return "6";
        case KeySEVEN:
            return "7";
        case KeyEIGHT:
            return "8";
        case KeyNINE:
            return "9";
        case KeyA:
            return "A";
        case KeyB:
            return "B";
        case KeyC:
            return "C";
        case KeyD:
            return "D";
        case KeyE:
            return "E";
        case KeyF:
            return "F";
        case KeyG:
            return "G";
        case KeyH:
            return "H";
        case KeyI:
            return "I";
        case KeyJ:
            return "J";
        case KeyK:
            return "K";
        case KeyL:
            return "L";
        case KeyM:
            return "M";
        case KeyN:
            return "N";
        case KeyO:
            return "O";
        case KeyP:
            return "P";
        case KeyQ:
            return "Q";
        case KeyR:
            return "R";
        case KeyS:
            return "S";
        case KeyT:
            return "T";
        case KeyU:
            return "U";
        case KeyV:
            return "V";
        case KeyW:
            return "W";
        case KeyX:
            return "X";
        case KeyY:
            return "Y";
        case KeyZ:
            return "Z";
        default:
            return "Invalid";
        }
    }
#endif // DEBUGGING
};

constexpr frozen::map<hash_t, id_t, INPUT_BINDINGS_AMOUNT>
InputID::s_cBindingNames =
{
    // Keys
    { ConstexprHash("LShift"),          KeyLEFTSHIFT    },
    { ConstexprHash("RShift"),          KeyRIGHTSHIFT   },
    { ConstexprHash("LCtrl"),           KeyLEFTCONTROL  },
    { ConstexprHash("RCtrl"),           KeyRIGHTCONTROL },
    { ConstexprHash("LAlt"),            KeyLEFTALT      },
    { ConstexprHash("RAlt"),            KeyRIGHTALT     },
    { ConstexprHash("Fn"),              KeyFUNCTION     },
    { ConstexprHash("LSuper"),          KeyLEFTSUPER    },
    { ConstexprHash("RSuper"),          KeyRIGHTSUPER   },
    { ConstexprHash("Enter"),           KeyENTER        },
    { ConstexprHash("Backspace"),       KeyBACKSPACE    },
    { ConstexprHash("Tab"),             KeyTAB          },
    { ConstexprHash("Space"),           KeySPACE        },
    { ConstexprHash("Escape"),          KeyESC          },
    // Mouse Buttons
    { ConstexprHash("LeftMouse"),       MouseLEFT       },
    { ConstexprHash("RightMouse"),      MouseRIGHT      },
    { ConstexprHash("MiddleMouse"),     MouseMIDDLE     },
    // Mouse Motion
    { ConstexprHash("MouseX"),          MouseMotionX    },
    { ConstexprHash("MouseY"),          MouseMotionY    },
    // Numbers
    { ConstexprHash("0"), KeyZERO  },
    { ConstexprHash("1"), KeyONE   },
    { ConstexprHash("2"), KeyTWO   },
    { ConstexprHash("3"), KeyTHREE },
    { ConstexprHash("4"), KeyFOUR  },
    { ConstexprHash("5"), KeyFIVE  },
    { ConstexprHash("6"), KeySIX   },
    { ConstexprHash("7"), KeySEVEN },
    { ConstexprHash("8"), KeyEIGHT },
    { ConstexprHash("9"), KeyNINE  },
    // Letters
    { ConstexprHash("A"), KeyA },
    { ConstexprHash("a"), KeyA },
    { ConstexprHash("B"), KeyB },
    { ConstexprHash("b"), KeyB },
    { ConstexprHash("C"), KeyC },
    { ConstexprHash("c"), KeyC },
    { ConstexprHash("D"), KeyD },
    { ConstexprHash("d"), KeyD },
    { ConstexprHash("E"), KeyE },
    { ConstexprHash("e"), KeyE },
    { ConstexprHash("F"), KeyF },
    { ConstexprHash("f"), KeyF },
    { ConstexprHash("G"), KeyG },
    { ConstexprHash("g"), KeyG },
    { ConstexprHash("H"), KeyH },
    { ConstexprHash("h"), KeyH },
    { ConstexprHash("I"), KeyI },
    { ConstexprHash("i"), KeyI },
    { ConstexprHash("J"), KeyJ },
    { ConstexprHash("j"), KeyJ },
    { ConstexprHash("K"), KeyK },
    { ConstexprHash("k"), KeyK },
    { ConstexprHash("L"), KeyL },
    { ConstexprHash("l"), KeyL },
    { ConstexprHash("M"), KeyM },
    { ConstexprHash("m"), KeyM },
    { ConstexprHash("N"), KeyN },
    { ConstexprHash("n"), KeyN },
    { ConstexprHash("O"), KeyO },
    { ConstexprHash("o"), KeyO },
    { ConstexprHash("P"), KeyP },
    { ConstexprHash("p"), KeyP },
    { ConstexprHash("Q"), KeyQ },
    { ConstexprHash("q"), KeyQ },
    { ConstexprHash("R"), KeyR },
    { ConstexprHash("r"), KeyR },
    { ConstexprHash("S"), KeyS },
    { ConstexprHash("s"), KeyS },
    { ConstexprHash("T"), KeyT },
    { ConstexprHash("t"), KeyT },
    { ConstexprHash("U"), KeyU },
    { ConstexprHash("u"), KeyU },
    { ConstexprHash("V"), KeyV },
    { ConstexprHash("v"), KeyV },
    { ConstexprHash("W"), KeyW },
    { ConstexprHash("w"), KeyW },
    { ConstexprHash("X"), KeyX },
    { ConstexprHash("x"), KeyX },
    { ConstexprHash("Y"), KeyY },
    { ConstexprHash("y"), KeyY },
    { ConstexprHash("Z"), KeyZ },
    { ConstexprHash("z"), KeyZ },
};

#endif // INPUT_BINDINGS_H
