#ifndef PRINTING_H
#define PRINTING_H

#include "error.hpp"

#include <print>
#include <source_location>

enum VERBOSITY {
    VERBOSE0 = 0,
    VERBOSE1 = 1,
    VERBOSE2 = 2,
    VERBOSE3 = 3,
};

inline constinit const char* __no_label{""};
inline constinit const char* __error_label{"\x1b[91m[ERROR] "};
inline constinit const char* __warning_label{"\x1b[38;5;220m[WARNING] "};

template<class... Args>
    bool __print_verbose(bool return_value, VERBOSITY verbosity, std::format_string<Args...> fmt, const std::source_location loc, const char* label = "", Args&&... args)
    {
        if(verbosity <= 0)
            { std::println("\x1b[1m{}\x1b[22m{}\x1b[0m", label, std::format(fmt, std::forward<Args>(args)...)); return return_value; }
        std::string location{label};
        std::string pieces[3] {
            std::format("file {}", loc.file_name()),
            std::format("({}:{})", loc.line(), loc.column()),
            std::format(" `{}`", loc.function_name()),
        };
        for(int i{0}; i < verbosity; ++i)
            { location += pieces[i]; }
        if(fmt.get().empty())
            { std::println("\x1b[1m{}\x1b[0m", location); }
        else
            { std::println("\x1b[1m{}: \x1b[22m{}\x1b[0m", location, std::format(fmt, std::forward<Args>(args)...)); }
        return return_value;
    }

#define print_verbose(Verbosity, Format, Args...) \
    __print_verbose(true, Verbosity, Format, std::source_location::current(), __no_label, ## Args)

// Always returns false. Use with: bad behaviour that leads to a crash/failure
#define print_error(Format, Args...) \
    __print_verbose(false, VERBOSE3, Format, std::source_location::current(), __error_label, ## Args)

// Always returns true. Use with: bad behaviour that doesn't lead to a crash/failure (not great, not terrible)
#define print_warning(Format, Args...) \
    __print_verbose(true, VERBOSE2, Format, std::source_location::current(), __warning_label, ## Args)

// Debug printouts are only enabled in the Debug build
#ifdef DEBUGGING
    inline constinit const char* __debug_label{"\x1b[1;36m[DEBUG] "};
    inline constinit const char* __app_label{"\x1b[1;35m[APPLICATION] "};
    inline constinit const char* __jolt_label{"\x1b[1;32m[JOLT] "};

//  Debug message that starts with the '[DEBUG]' label
#   define print_debug(fmt, args...) \
        __print_verbose(true, VERBOSE0, fmt, std::source_location::current(), __debug_label, ## args)

//  Verbose debug message that starts with the '[DEBUG]' label
#   define print_debugv(verbosity, fmt, args...) \
        __print_verbose(true, verbosity, fmt, std::source_location::current(), __debug_label, ## args)

//  When `DEBUGGING` is undefined this will be empty, keeping debug string literals out of the release binary
#   define debug_print(fmt, args...) \
        std::println(fmt, ## args)

//  Debug message that starts with the '[APPLICATION]' label
#   define print_app(fmt, args...) \
        __print_verbose(true, VERBOSE0, fmt, std::source_location::current(), __app_label, ## args)

//  Verbose debug message that starts with the '[APPLICATION]' label
#   define print_appv(verbosity, fmt, args...) \
        __print_verbose(true, verbosity, fmt, std::source_location::current(), __app_label, ## args)

//  Debug message that starts with the '[JOLT]' label
#   define print_jolt(fmt, args...) \
        __print_verbose(true, VERBOSE0, fmt, std::source_location::current(), __jolt_label, ## args)

//  Verbose debug message that starts with the '[JOLT]' label
#   define print_joltv(verbosity, fmt, args...) \
        __print_verbose(true, verbosity, fmt, std::source_location::current(), __jolt_label, ## args)

#else  // !DEBUGGING
    consteval bool __print_function_disabled(bool ret = true) { return ret; }
#   define print_debug(...)  __print_function_disabled()
#   define print_debugv(...) __print_function_disabled()
#   define debug_print(...)  __print_function_disabled()
#   define print_app(...)    __print_function_disabled()
#   define print_appv(...)   __print_function_disabled()
#   define print_jolt(...)   __print_function_disabled()
#   define print_joltv(...)  __print_function_disabled()
#endif // DEBUGGING

#define print_func(CONTEXT...) \
    print_debug(#CONTEXT "{}", __FUNCTION__)

inline Error print_error_enum(Error error, VERBOSITY verbosity = VERBOSE3, bool unimplemented_returns_ok = true, const std::source_location location = std::source_location::current())
{
#   define Case(ERROR, MESSAGE, OTHER...) case ERROR: err_msg = #ERROR" (" MESSAGE ")"; OTHER break;
    std::string err_msg{""};
    switch(error)
    {
    default: error = FAILED; err_msg = "well, congrats, you've managed to royally fuck up the error-printing function (you probably just passed it a negative integer instead of an Error, or some shit like that)"; break;
    case OK: return OK;
    Case(UNIMPLEMENTED, "some functionality that hasn't been implemented yet")
    Case(FAILED, "unspecified error")
    Case(ERR_INIT_FAILED, "failure during object initialization. Look for any `::Init` or `::Initialize` class methods")
    Case(ERR_INIT_PARTIALLY_FAILED, "partial failure during object initialization. Look for any `::Init` or `::Initialize` class methods")
    Case(ERR_SWITCH_DEFAULT, "a `switch` statement's `default:` case was met and is considered an error")
    Case(ERR_INDEX_OUT_OF_BOUNDS, "generic 'index out of bounds' error")
    Case(ERR_NOT_FOUND, "generic search error")
    Case(ERR_FILE, "generic file error")
    Case(ERR_FILE_LOAD, "`FileData::LoadFile` error")
    Case(ERR_FILE_READ, "file read error")
    Case(ERR_FILE_WRITE, "file write error")
    Case(ERR_FILE_READ_WRITE, "file read/write error")
    }
    __print_verbose(false, verbosity, "{}", location,
        (error == UNIMPLEMENTED && unimplemented_returns_ok) ? __warning_label : __error_label,
        err_msg);
    return (error == UNIMPLEMENTED && unimplemented_returns_ok) ? OK : error;
#   undef Case
}

#endif // PRINTING_H
