/*
standard library maps for converting input IDs from various windowing libraries to Nostalgia's custom input IDs
---------------------------------------------------------------------------------------------------------------
*/

#include "input/key.hpp"

// Headers required for 'GLFW_Backend::key_ids'
#include "glfw.hpp"
#include "GLFW/glfw3.h"

const std::map<GLFW_Backend::GLFW_KeyID, KeyID> GLFW_Backend::key_ids =
{
    { GLFW_KEY_0,             Key::ZERO         },
    { GLFW_KEY_1,             Key::ONE          },
    { GLFW_KEY_2,             Key::TWO          },
    { GLFW_KEY_3,             Key::THREE        },
    { GLFW_KEY_4,             Key::FOUR         },
    { GLFW_KEY_5,             Key::FIVE         },
    { GLFW_KEY_6,             Key::SIX          },
    { GLFW_KEY_7,             Key::SEVEN        },
    { GLFW_KEY_8,             Key::EIGHT        },
    { GLFW_KEY_9,             Key::NINE         },
    { GLFW_KEY_Q,             Key::Q            },
    { GLFW_KEY_W,             Key::W            },
    { GLFW_KEY_E,             Key::E            },
    { GLFW_KEY_R,             Key::R            },
    { GLFW_KEY_T,             Key::T            },
    { GLFW_KEY_Y,             Key::Y            },
    { GLFW_KEY_U,             Key::U            },
    { GLFW_KEY_I,             Key::I            },
    { GLFW_KEY_O,             Key::O            },
    { GLFW_KEY_P,             Key::P            },
    { GLFW_KEY_A,             Key::A            },
    { GLFW_KEY_S,             Key::S            },
    { GLFW_KEY_D,             Key::D            },
    { GLFW_KEY_F,             Key::F            },
    { GLFW_KEY_G,             Key::G            },
    { GLFW_KEY_H,             Key::H            },
    { GLFW_KEY_J,             Key::J            },
    { GLFW_KEY_K,             Key::K            },
    { GLFW_KEY_L,             Key::L            },
    { GLFW_KEY_Z,             Key::Z            },
    { GLFW_KEY_X,             Key::X            },
    { GLFW_KEY_C,             Key::C            },
    { GLFW_KEY_V,             Key::V            },
    { GLFW_KEY_B,             Key::B            },
    { GLFW_KEY_N,             Key::N            },
    { GLFW_KEY_M,             Key::M            },
    { GLFW_KEY_LEFT_SHIFT,    Key::LEFTSHIFT    },
    { GLFW_KEY_RIGHT_SHIFT,   Key::RIGHTSHIFT   },
    { GLFW_KEY_LEFT_CONTROL,  Key::LEFTCONTROL  },
    { GLFW_KEY_RIGHT_CONTROL, Key::RIGHTCONTROL },
    { GLFW_KEY_LEFT_ALT,      Key::LEFTALT      },
    { GLFW_KEY_RIGHT_ALT,     Key::RIGHTALT     },
    { GLFW_KEY_LEFT_SUPER,    Key::LEFTSUPER    },
    { GLFW_KEY_RIGHT_SUPER,   Key::RIGHTSUPER   },
    { GLFW_KEY_ENTER,         Key::ENTER        },
    { GLFW_KEY_BACKSPACE,     Key::BACKSPACE    },
    { GLFW_KEY_TAB,           Key::TAB          },
    { GLFW_KEY_SPACE,         Key::SPACE        },
    { GLFW_KEY_ESCAPE,        Key::ESC          },
};
