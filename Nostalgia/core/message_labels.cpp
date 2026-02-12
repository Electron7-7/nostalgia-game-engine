#ifdef NOSTALGIA_GCC_PCH
#   include <Nostalgia/Nostalgia.hpp>
#endif

MessageLabel ErrorLabel{"[ERROR]",
    ANSI_Sequence{ANSI::begin,
        ANSI::bold_background,
        ANSI::red,
        ANSI::next,
        ANSI::bold_foreground,
        ANSI::default_color,
        ANSI::end},
    "ErrorLabel"};

MessageLabel WarningLabel{"[WARNING]",
    ANSI_Sequence{ANSI::begin,
        48, ANSI::next, 5, ANSI::next, 166, // 256 bit background color code
        ANSI::end},
    "WarningLabel"};

MessageLabel DebugLabel{"[DEBUG]",
    ANSI_Sequence{ANSI::default_color, true, true},
    "DebugLabel"};

MessageLabel JoltLabel{"[JOLT]",
    ANSI_Sequence{ANSI::magenta, true, true},
    "JoltLabel"};

MessageLabel FunctionLabel{"(FUNCTION)",
    ANSI_Sequence{ANSI::blue, true, true},
    "FunctionLabel"};

MessageLabel ConstructorLabel{"(CONSTRUCTOR)",
    ANSI_Sequence{ANSI::begin,
        ANSI::set_underline,
        ANSI::next,
        ANSI::bold_foreground,
        ANSI::magenta,
        ANSI::end},
    "ConstructorLabel"};

MessageLabel DestructorLabel{"(DESTRUCTOR)",
    ANSI_Sequence{ANSI::begin,
        ANSI::set_underline,
        ANSI::next,
        ANSI::bold_foreground,
        ANSI::magenta,
        ANSI::end},
    "DestructorLabel"};

MessageLabel* __all_labels_for_debugging[7]
{
    &ErrorLabel,
    &WarningLabel,
    &DebugLabel,
    &JoltLabel,
    &FunctionLabel,
    &ConstructorLabel,
    &DestructorLabel,
};

MessageLabel::MessageLabel() noexcept = default;

MessageLabel::MessageLabel(Sarg inLabel, const ANSI_Sequence& inColorCode, const char* inDebugName) noexcept:
    name_{inDebugName}, label_{inLabel}, color_{inColorCode} {}

void MessageLabel::set_ansi_sequence(const ANSI_Sequence& inSequence)
{ color_ = inSequence; }

const char* MessageLabel::name() noexcept
{ return name_;  }

std::string& MessageLabel::label() noexcept
{ return label_; }

ANSI_Sequence& MessageLabel::color() noexcept
{ return color_; }

std::string MessageLabel::full() const noexcept
{ return ((enable_ansi_sequence) ? color_() : "") + label_ + " "; }

std::string& MessageLabel::operator=(Sarg inString) noexcept
{ return label_ = inString; }

std::string& MessageLabel::operator+=(Sarg inString) noexcept
{ return label_ += inString; }

std::string MessageLabel::operator+(Sarg inString) noexcept
{ return label_ + inString; }

std::string MessageLabel::operator()() const noexcept
{ return full(); }

std::string MessageLabel::log() const noexcept
{
    std::string output{};
    for(FAUTO full_code : color_.data())
    {
        for(int code : full_code)
            { output += ANSI_Sequence::convert_code(code); }
    }
    return output;
}
