#ifndef MESSAGE_LABELS_H
#define MESSAGE_LABELS_H

#include "ansi_sequence.hpp"

struct MessageLabel
{
public:
    MessageLabel() noexcept;
    MessageLabel(const std::string& inLabel, const ANSI_Sequence& inColorCode = {}, const char* inDebugName = "N/A") noexcept;

    bool enable_ansi_sequence{true};

    void set_ansi_sequence(const ANSI_Sequence& inSequence);

    const char*     name() noexcept;
    std::string&   label() noexcept;
    ANSI_Sequence& color() noexcept;
    std::string     full() const noexcept;

    std::string operator()() const noexcept;
    std::string log() const noexcept;

    std::string& operator=(const std::string& inString) noexcept;
    std::string& operator+=(const std::string& inString) noexcept;
    std::string  operator+(const std::string& inString) noexcept;

private:
    const char*   name_{"N/A"};
    std::string   label_{};
    ANSI_Sequence color_{ANSI::begin, ANSI::end};
};

extern MessageLabel ErrorLabel;
extern MessageLabel WarningLabel;
extern MessageLabel DebugLabel;
extern MessageLabel JoltLabel;
extern MessageLabel FunctionLabel;
extern MessageLabel ConstructorLabel;
extern MessageLabel DestructorLabel;

extern MessageLabel* __all_labels_for_debugging[7];

#endif // MESSAGE_LABELS_H
