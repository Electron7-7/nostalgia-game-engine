#ifndef PRINTING_H
#define PRINTING_H

#include "message_labels.hpp"
#include <print>
#include <source_location>

enum VERBOSITY : int {
    /// Nothing
    VERBOSE0 = 0b000,
    /// `file <filename>`
    VERBOSE1 = 0b001,
    /// `(<line>:<column>)`
    VERBOSE2 = 0b010,
    /// `<function_name>`
    VERBOSE3 = 0b100,
    /// `VERBOSE1 | VERBOSE2 | VERBOSE3`
    FULLY_VERBOSE = 0b111,
};

using VerbosityFlag = int;

template<class... Args>
    bool __print_verbose(bool return_value,
        VerbosityFlag verbosity,
        std::format_string<Args...> fmt,
        const std::source_location loc,
        const MessageLabel& label = {},
        Args&&... args)
    {
        static std::string prefixes[3] {
            std::format("file {} ", loc.file_name()),
            std::format("({}:{}) ", loc.line(), loc.column()),
            std::format("`{}` ", loc.function_name()),
        };

        if(verbosity == VERBOSE0)
        {
            std::println("{}{}{}{}",
                label(),
                (label.enable_ansi_sequence) ? "\x1b[22m" : "",
                std::format(fmt, std::forward<Args>(args)...),
                (label.enable_ansi_sequence) ? "\x1b[0m" : "");
            return return_value;
        }

        std::string verbose_prefix{};
        if(verbosity & VERBOSE1)
            { verbose_prefix += prefixes[0]; }
        if(verbosity & VERBOSE2)
            { verbose_prefix += prefixes[1]; }
        if(verbosity & VERBOSE3)
            { verbose_prefix += prefixes[2]; }

        std::println("{}{}{}{}\x1b[0m",
            label(),
            verbose_prefix,
            (label.enable_ansi_sequence) ? "\x1b[22m" : "",
            std::format(fmt, std::forward<Args>(args)...));
        return return_value;
    }

// Always returns false. Use with: bad behaviour that leads to a crash/failure
#define print_error(Format, Args...) \
    __print_verbose(false, VERBOSE1 | VERBOSE2 | VERBOSE3, Format, std::source_location::current(), ErrorLabel, ## Args)

// Version of `print_error` that lets you control the verbosity
#define print_errorv(Verbosity, Format, Args...) \
    __print_verbose(false, Verbosity, Format, std::source_location::current(), ErrorLabel, ## Args)

// Always returns true. Use with: bad behaviour that doesn't lead to a crash/failure (not great, not terrible)
#define print_warning(Format, Args...) \
    __print_verbose(true, VERBOSE0, Format, std::source_location::current(), WarningLabel, ## Args)

// Version of `print_warning` that lets you control the verbosity
#define print_warningv(Verbosity, Format, Args...) \
    __print_verbose(true, Verbosity, Format, std::source_location::current(), WarningLabel, ## Args)

// Debug printouts are only enabled in the Debug build
#ifdef DEBUGGING
//  Prints the name of the function that this macro is called in (using `__PRETTY_FUNCTION__`)
#   define PRINT_PRETTY_FUNCTION \
        PRINT_PRETTY_FUNCTION_EXT()
#   define PRINT_PRETTY_FUNCTION_EXT(FMT, ARGS...) \
        __print_verbose(true, VERBOSE0, "{}" FMT, std::source_location::current(), FunctionLabel, __PRETTY_FUNCTION__, ## ARGS)

//  Same as `PRINT_PRETTY_FUNCTION` but unique to constructors
#   define PRINT_PRETTY_CONSTRUCTOR \
        PRINT_PRETTY_CONSTRUCTOR_EXT("this: {}", static_cast<void*>(this))
#   define PRINT_PRETTY_CONSTRUCTOR_EXT(FMT, ARGS...) \
        __print_verbose(true, VERBOSE0, "{} " FMT, std::source_location::current(), ConstructorLabel, __PRETTY_FUNCTION__, ## ARGS)

//  Same as `PRINT_PRETTY_FUNCTION` but unique to destructors
#   define PRINT_PRETTY_DESTRUCTOR \
        PRINT_PRETTY_DESTRUCTOR_EXT("this: {}", static_cast<void*>(this))
#   define PRINT_PRETTY_DESTRUCTOR_EXT(FMT, ARGS...) \
        __print_verbose(true, VERBOSE0, "{} " FMT, std::source_location::current(), DestructorLabel, __PRETTY_FUNCTION__, ## ARGS)

//  Debug message that starts with the '[DEBUG]' label
#   define print_debug(fmt, args...) \
        __print_verbose(true, VERBOSE0, fmt, std::source_location::current(), DebugLabel, ## args)

//  Verbose debug message that starts with the '[DEBUG]' label
#   define print_debugv(verbosity, fmt, args...) \
        __print_verbose(true, verbosity, fmt, std::source_location::current(), DebugLabel, ## args)

//  When `DEBUGGING` is undefined this will be empty, keeping debug string literals out of the release binary
#   define debug_print(fmt, args...) \
        std::println(fmt, ## args)

//  Debug message that starts with the '[JOLT]' label
#   define print_jolt(fmt, args...) \
        __print_verbose(true, VERBOSE0, fmt, std::source_location::current(), JoltLabel, ## args)

//  Verbose debug message that starts with the '[JOLT]' label
#   define print_joltv(verbosity, fmt, args...) \
        __print_verbose(true, verbosity, fmt, std::source_location::current(), JoltLabel, ## args)

#else  // !DEBUGGING
    consteval bool __print_function_disabled() { return true; }
#   define PRINT_PRETTY_FUNCTION    __print_function_disabled()
#   define PRINT_PRETTY_CONSTRUCTOR __print_function_disabled()
#   define PRINT_PRETTY_DESTRUCTOR  __print_function_disabled()
#   define PRINT_PRETTY_FUNCTION_EXT(...) __print_function_disabled()
#   define PRINT_PRETTY_CONSTRUCTOR_EXT(...) __print_function_disabled()
#   define PRINT_PRETTY_DESTRUCTOR_EXT(...)  __print_function_disabled()
#   define print_debug(...)         __print_function_disabled()
#   define print_debugv(...)        __print_function_disabled()
#   define debug_print(...)         __print_function_disabled()
#   define print_jolt(...)          __print_function_disabled()
#   define print_joltv(...)         __print_function_disabled()
#endif // DEBUGGING

#endif // PRINTING_H
