#ifndef ANSI_CODES_H
#define ANSI_CODES_H

#include <string>
#include <vector>
#include <initializer_list>

using ansi_t = int;

// string ex: '\x1b[1;31m'
using ansi_code_t = std::vector<ansi_t>;

// string ex: '\x1b[1;31m\x1b[0m\x1b[42m'
using ansi_sequence_t = std::vector<ansi_code_t>;

template<typename T>
    concept ansi_t_strict = std::same_as<T, ansi_t> and not std::same_as<T, bool>;

namespace ANSI
{
    constexpr ansi_t
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

struct ANSI_Sequence
{
public:
    ANSI_Sequence() noexcept;
    ANSI_Sequence(ansi_t inColorCode, bool isForeground, bool isBold = false) noexcept;

    template<ansi_t_strict... Codes>
        ANSI_Sequence(Codes... inCodes) noexcept
        { setup_codes({inCodes...}); }

    static std::string convert_code(ansi_t inCode) noexcept;

    void operator()(std::string& outString, bool doClearStringFirst = true) const noexcept;
    std::string operator()() const noexcept;

    void get(std::string& outString, bool doClearStringFirst = true) const noexcept;
    std::string get() const noexcept;

    void append_to(std::string& outString) const noexcept;
    std::string append(const std::string& inString) const noexcept;

    void prepend_to(std::string& outString) const noexcept;
    std::string prepend(const std::string& inString) const noexcept;

    void insert_in(std::string& outString, size_t inInsertIndex = 0) const noexcept;
    std::string insert(const std::string& inString, size_t inInsertIndex = 0) const noexcept;

    const ansi_sequence_t& data() const noexcept;

private:
    ansi_sequence_t codes_{};

    void setup_codes(std::initializer_list<ansi_t> inCodes) noexcept;
};

#endif // ANSI_CODES_H
