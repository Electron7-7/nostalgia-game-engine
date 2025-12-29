#ifndef ANSI_CODES_H
#define ANSI_CODES_H

#include <concepts>

using ansi_t = int;

template<typename T>
    concept ansi_t_strict = std::same_as<T, ansi_t> and not std::same_as<T, bool>;

namespace ANSI
{
    constinit const ansi_t
        begin{-1}, next{-2}, end{-3},

        set_bold{1}, set_dim{2}, set_italic{3}, set_underline{4}, set_blink{5}, set_inverse{7},
        set_hidden{8}, set_strikethrough{9},

        reset_all{0},

        reset_bold{22}, reset_dim{22}, reset_italic{23}, reset_underline{24}, reset_blink{25}, reset_inverse{27},
        reset_hidden{28}, reset_strikethrough{29},

        foreground{3}, background{4}, bold_foreground{9}, bold_background{10},

        black{0}, red{1}, green{2}, yellow{3}, blue{4}, magenta{5}, cyan{6}, white{7}, default_color{9},

        invalid{-34};
};

#endif // ANSI_CODES_H
