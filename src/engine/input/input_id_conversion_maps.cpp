/*
standard library maps for comparing input IDs from various windowing libraries to Nostalgia's custom input IDs
--------------------------------------------------------------------------------------------------------------
*/

#include "engine/input/key_handler.hpp"
#include "engine/backends/windowing/glfw.hpp"
#include "opengl_includes.hpp" // IWYU pragma: keep // Used by GLFW_Backend::key_id_map

const std::map<GLFW_Backend::GLFW_KeyID, KeyID> GLFW_Backend::key_id_map =
{
    { GLFW_KEY_0,             KEY::ZERO         },
    { GLFW_KEY_1,             KEY::ONE          },
    { GLFW_KEY_2,             KEY::TWO          },
    { GLFW_KEY_3,             KEY::THREE        },
    { GLFW_KEY_4,             KEY::FOUR         },
    { GLFW_KEY_5,             KEY::FIVE         },
    { GLFW_KEY_6,             KEY::SIX          },
    { GLFW_KEY_7,             KEY::SEVEN        },
    { GLFW_KEY_8,             KEY::EIGHT        },
    { GLFW_KEY_9,             KEY::NINE         },
    { GLFW_KEY_Q,             KEY::Q            },
    { GLFW_KEY_W,             KEY::W            },
    { GLFW_KEY_E,             KEY::E            },
    { GLFW_KEY_R,             KEY::R            },
    { GLFW_KEY_T,             KEY::T            },
    { GLFW_KEY_Y,             KEY::Y            },
    { GLFW_KEY_U,             KEY::U            },
    { GLFW_KEY_I,             KEY::I            },
    { GLFW_KEY_O,             KEY::O            },
    { GLFW_KEY_P,             KEY::P            },
    { GLFW_KEY_A,             KEY::A            },
    { GLFW_KEY_S,             KEY::S            },
    { GLFW_KEY_D,             KEY::D            },
    { GLFW_KEY_F,             KEY::F            },
    { GLFW_KEY_G,             KEY::G            },
    { GLFW_KEY_H,             KEY::H            },
    { GLFW_KEY_J,             KEY::J            },
    { GLFW_KEY_K,             KEY::K            },
    { GLFW_KEY_L,             KEY::L            },
    { GLFW_KEY_Z,             KEY::Z            },
    { GLFW_KEY_X,             KEY::X            },
    { GLFW_KEY_C,             KEY::C            },
    { GLFW_KEY_V,             KEY::V            },
    { GLFW_KEY_B,             KEY::B            },
    { GLFW_KEY_N,             KEY::N            },
    { GLFW_KEY_M,             KEY::M            },
    { GLFW_KEY_LEFT_SHIFT,    KEY::LEFTSHIFT    },
    { GLFW_KEY_RIGHT_SHIFT,   KEY::RIGHTSHIFT   },
    { GLFW_KEY_LEFT_CONTROL,  KEY::LEFTCONTROL  },
    { GLFW_KEY_RIGHT_CONTROL, KEY::RIGHTCONTROL },
    { GLFW_KEY_LEFT_ALT,      KEY::LEFTALT      },
    { GLFW_KEY_RIGHT_ALT,     KEY::RIGHTALT     },
    { GLFW_KEY_LEFT_SUPER,    KEY::LEFTSUPER    },
    { GLFW_KEY_RIGHT_SUPER,   KEY::RIGHTSUPER   },
    { GLFW_KEY_ENTER,         KEY::ENTER        },
    { GLFW_KEY_BACKSPACE,     KEY::BACKSPACE    },
    { GLFW_KEY_TAB,           KEY::TAB          },
    { GLFW_KEY_SPACE,         KEY::SPACE        },
    { GLFW_KEY_ESCAPE,        KEY::ESC          },
};