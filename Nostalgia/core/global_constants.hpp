#ifndef GLOBAL_CONSTANTS_H
#define GLOBAL_CONSTANTS_H

namespace GlobalConstants
{
    /// Ready at compile time
    namespace Init
    {
        constinit const char cstr_empty[]{""},
            cstr_NA[]{"N/A"},
            cstr_Invalid[]{"Invalid"},
            cstr_true[]{"true"},
            cstr_false[]{"false"};
    }

    const std::string str_empty{Init::cstr_empty},
        str_NA{Init::cstr_NA},
        str_Invalid{Init::cstr_Invalid},
        str_true{Init::cstr_true},
        str_false{Init::cstr_false};
}

#endif // GLOBAL_CONSTANTS_H
