#ifndef GLFW_BINDINGS_H
#define GLFW_BINDINGS_H

#include "core/id.hpp"

#include <GLFW/glfw3.h>

using namespace BindingID;

constexpr frozen::map<ID, uint, BindingID::KeyIDsCount + BindingID::MouseButtonIDsCount - 1>
s_mGLFWKeyLookup =
{
    { KeyZERO,         GLFW_KEY_0               },
    { KeyONE,          GLFW_KEY_1               },
    { KeyTWO,          GLFW_KEY_2               },
    { KeyTHREE,        GLFW_KEY_3               },
    { KeyFOUR,         GLFW_KEY_4               },
    { KeyFIVE,         GLFW_KEY_5               },
    { KeySIX,          GLFW_KEY_6               },
    { KeySEVEN,        GLFW_KEY_7               },
    { KeyEIGHT,        GLFW_KEY_8               },
    { KeyNINE,         GLFW_KEY_9               },
    { KeyA,            GLFW_KEY_A               },
    { KeyB,            GLFW_KEY_B               },
    { KeyC,            GLFW_KEY_C               },
    { KeyD,            GLFW_KEY_D               },
    { KeyE,            GLFW_KEY_E               },
    { KeyF,            GLFW_KEY_F               },
    { KeyG,            GLFW_KEY_G               },
    { KeyH,            GLFW_KEY_H               },
    { KeyI,            GLFW_KEY_I               },
    { KeyJ,            GLFW_KEY_J               },
    { KeyK,            GLFW_KEY_K               },
    { KeyL,            GLFW_KEY_L               },
    { KeyM,            GLFW_KEY_M               },
    { KeyN,            GLFW_KEY_N               },
    { KeyO,            GLFW_KEY_O               },
    { KeyP,            GLFW_KEY_P               },
    { KeyQ,            GLFW_KEY_Q               },
    { KeyR,            GLFW_KEY_R               },
    { KeyS,            GLFW_KEY_S               },
    { KeyT,            GLFW_KEY_T               },
    { KeyU,            GLFW_KEY_U               },
    { KeyV,            GLFW_KEY_V               },
    { KeyW,            GLFW_KEY_W               },
    { KeyX,            GLFW_KEY_X               },
    { KeyY,            GLFW_KEY_Y               },
    { KeyZ,            GLFW_KEY_Z               },
    { KeyLEFTSHIFT,    GLFW_KEY_LEFT_SHIFT      },
    { KeyRIGHTSHIFT,   GLFW_KEY_RIGHT_SHIFT     },
    { KeyLEFTCONTROL,  GLFW_KEY_LEFT_CONTROL    },
    { KeyRIGHTCONTROL, GLFW_KEY_RIGHT_CONTROL   },
    { KeyLEFTALT,      GLFW_KEY_LEFT_ALT        },
    { KeyRIGHTALT,     GLFW_KEY_RIGHT_ALT       },
    { KeyLEFTSUPER,    GLFW_KEY_LEFT_SUPER      },
    { KeyRIGHTSUPER,   GLFW_KEY_RIGHT_SUPER     },
    { KeyENTER,        GLFW_KEY_ENTER           },
    { KeyBACKSPACE,    GLFW_KEY_BACKSPACE       },
    { KeyTAB,          GLFW_KEY_TAB             },
    { KeySPACE,        GLFW_KEY_SPACE           },
    { KeyESC,          GLFW_KEY_ESCAPE          },
    { MouseLEFT,       GLFW_MOUSE_BUTTON_LEFT   },
    { MouseRIGHT,      GLFW_MOUSE_BUTTON_RIGHT  },
    { MouseMIDDLE,     GLFW_MOUSE_BUTTON_MIDDLE },
};

#endif // GLFW_BINDINGS_H
