#ifndef KEY_H
#define KEY_H

#include <climits>
#include <string>
#include <map>

typedef unsigned int KeyID;

namespace Key
{
    enum class Action
    {
        Released = 0b00,
        Pressed  = 0b01,
        Repeated = 0b10
    };

    static constexpr unsigned short HandledKeysLastIndex = 0x31; // Update this number as more keys are added

    static constexpr KeyID INVALID      = USHRT_MAX;
    static constexpr KeyID ZERO         = 0x00;
    static constexpr KeyID ONE          = 0x01;
    static constexpr KeyID TWO          = 0x02;
    static constexpr KeyID THREE        = 0x03;
    static constexpr KeyID FOUR         = 0x04;
    static constexpr KeyID FIVE         = 0x05;
    static constexpr KeyID SIX          = 0x06;
    static constexpr KeyID SEVEN        = 0x07;
    static constexpr KeyID EIGHT        = 0x08;
    static constexpr KeyID NINE         = 0x09;
    static constexpr KeyID Q            = 0x0A;
    static constexpr KeyID W            = 0x0B;
    static constexpr KeyID E            = 0x0C;
    static constexpr KeyID R            = 0x0D;
    static constexpr KeyID T            = 0x0E;
    static constexpr KeyID Y            = 0x0F;
    static constexpr KeyID U            = 0x10;
    static constexpr KeyID I            = 0x11;
    static constexpr KeyID O            = 0x12;
    static constexpr KeyID P            = 0x13;
    static constexpr KeyID A            = 0x14;
    static constexpr KeyID S            = 0x15;
    static constexpr KeyID D            = 0x16;
    static constexpr KeyID F            = 0x17;
    static constexpr KeyID G            = 0x18;
    static constexpr KeyID H            = 0x19;
    static constexpr KeyID J            = 0x1A;
    static constexpr KeyID K            = 0x1B;
    static constexpr KeyID L            = 0x1C;
    static constexpr KeyID Z            = 0x1D;
    static constexpr KeyID X            = 0x1E;
    static constexpr KeyID C            = 0x1F;
    static constexpr KeyID V            = 0x20;
    static constexpr KeyID B            = 0x21;
    static constexpr KeyID N            = 0x22;
    static constexpr KeyID M            = 0x23;
    static constexpr KeyID LEFTSHIFT    = 0x24;
    static constexpr KeyID RIGHTSHIFT   = 0x25;
    static constexpr KeyID LEFTCONTROL  = 0x26;
    static constexpr KeyID RIGHTCONTROL = 0x27;
    static constexpr KeyID LEFTALT      = 0x28;
    static constexpr KeyID RIGHTALT     = 0x29;
    static constexpr KeyID FUNCTION     = 0x2A;
    static constexpr KeyID LEFTSUPER    = 0x2B;
    static constexpr KeyID RIGHTSUPER   = 0x2C;
    static constexpr KeyID ENTER        = 0x2D;
    static constexpr KeyID BACKSPACE    = 0x3E;
    static constexpr KeyID TAB          = 0x3F;
    static constexpr KeyID SPACE        = 0x30;
    static constexpr KeyID ESC          = 0x31;
}

inline const std::map<std::string, KeyID> key_strings_map =
{
    { "0",         Key::ZERO         },
    { "1",         Key::ONE          },
    { "2",         Key::TWO          },
    { "3",         Key::THREE        },
    { "4",         Key::FOUR         },
    { "5",         Key::FIVE         },
    { "6",         Key::SIX          },
    { "7",         Key::SEVEN        },
    { "8",         Key::EIGHT        },
    { "9",         Key::NINE         },
    { "q",         Key::Q            },
    { "Q",         Key::Q            },
    { "w",         Key::W            },
    { "W",         Key::W            },
    { "e",         Key::E            },
    { "E",         Key::E            },
    { "r",         Key::R            },
    { "R",         Key::R            },
    { "t",         Key::T            },
    { "T",         Key::T            },
    { "y",         Key::Y            },
    { "Y",         Key::Y            },
    { "u",         Key::U            },
    { "U",         Key::U            },
    { "i",         Key::I            },
    { "I",         Key::I            },
    { "o",         Key::O            },
    { "O",         Key::O            },
    { "p",         Key::P            },
    { "P",         Key::P            },
    { "a",         Key::A            },
    { "A",         Key::A            },
    { "s",         Key::S            },
    { "S",         Key::S            },
    { "d",         Key::D            },
    { "D",         Key::D            },
    { "f",         Key::F            },
    { "F",         Key::F            },
    { "g",         Key::G            },
    { "G",         Key::G            },
    { "h",         Key::H            },
    { "H",         Key::H            },
    { "j",         Key::J            },
    { "J",         Key::J            },
    { "k",         Key::K            },
    { "K",         Key::K            },
    { "l",         Key::L            },
    { "L",         Key::L            },
    { "z",         Key::Z            },
    { "Z",         Key::Z            },
    { "x",         Key::X            },
    { "X",         Key::X            },
    { "c",         Key::C            },
    { "C",         Key::C            },
    { "v",         Key::V            },
    { "V",         Key::V            },
    { "b",         Key::B            },
    { "B",         Key::B            },
    { "n",         Key::N            },
    { "N",         Key::N            },
    { "m",         Key::M            },
    { "M",         Key::M            },
    { "LShift",    Key::LEFTSHIFT    },
    { "RShift",    Key::RIGHTSHIFT   },
    { "LCtrl",     Key::LEFTCONTROL  },
    { "RCtrl",     Key::RIGHTCONTROL },
    { "LAlt",      Key::LEFTALT      },
    { "RAlt",      Key::RIGHTALT     },
    { "Fn",        Key::FUNCTION     },
    { "LSuper",    Key::LEFTSUPER    },
    { "RSuper",    Key::RIGHTSUPER   },
    { "Enter",     Key::ENTER        },
    { "Backspace", Key::BACKSPACE    },
    { "Tab",       Key::TAB          },
    { "Space",     Key::SPACE        },
    { "Escape",    Key::ESC          },
};

#endif // KEY_H
