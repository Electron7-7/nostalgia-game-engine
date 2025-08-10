#ifndef PRINTING_H
#define PRINTING_H

#include <string>
#include <print> // IWYU pragma: keep // Include this header because if you're including the "printing" header, then you probably wanna print stuff, huh?

struct Color
{
    constexpr static const char* Reset   = "\x1b[0m";

    struct Front
    {
        constexpr static const char* Black   = "\x1b[30m";
        constexpr static const char* Red     = "\x1b[31m";
        constexpr static const char* Green   = "\x1b[32m";
        constexpr static const char* Yellow  = "\x1b[33m";
        constexpr static const char* Blue    = "\x1b[34m";
        constexpr static const char* Magenta = "\x1b[35m";
        constexpr static const char* Cyan    = "\x1b[36m";
        constexpr static const char* White   = "\x1b[37m";
        constexpr static const char* Default = "\x1b[39m";

        constexpr static const char* BlackBold   = "\x1b[1;30m";
        constexpr static const char* RedBold     = "\x1b[1;31m";
        constexpr static const char* GreenBold   = "\x1b[1;32m";
        constexpr static const char* YellowBold  = "\x1b[1;33m";
        constexpr static const char* BlueBold    = "\x1b[1;34m";
        constexpr static const char* MagentaBold = "\x1b[1;35m";
        constexpr static const char* CyanBold    = "\x1b[1;36m";
        constexpr static const char* WhiteBold   = "\x1b[1;37m";
        constexpr static const char* DefaultBold = "\x1b[1;39m";
    };

    struct Back
    {
        constexpr static const char* Black   = "\x1b[40m";
        constexpr static const char* Red     = "\x1b[41m";
        constexpr static const char* Green   = "\x1b[42m";
        constexpr static const char* Yellow  = "\x1b[43m";
        constexpr static const char* Blue    = "\x1b[44m";
        constexpr static const char* Magenta = "\x1b[45m";
        constexpr static const char* Cyan    = "\x1b[46m";
        constexpr static const char* White   = "\x1b[47m";
        constexpr static const char* Default = "\x1b[49m";

        constexpr static const char* BlackBold   = "\x1b[2;40m";
        constexpr static const char* RedBold     = "\x1b[2;41m";
        constexpr static const char* GreenBold   = "\x1b[2;42m";
        constexpr static const char* YellowBold  = "\x1b[2;43m";
        constexpr static const char* BlueBold    = "\x1b[2;44m";
        constexpr static const char* MagentaBold = "\x1b[2;45m";
        constexpr static const char* CyanBold    = "\x1b[2;46m";
        constexpr static const char* WhiteBold   = "\x1b[2;47m";
        constexpr static const char* DefaultBold = "\x1b[2;49m";
    };
};

const std::string ERROR = Color::Front::RedBold     + std::string("[ERROR]")               ;
const std::string WARN  = Color::Front::YellowBold  + std::string("[WARNING]")             ;
const std::string DEBUG = Color::Front::DefaultBold + std::string("[DEBUG]") + Color::Reset;

// Useful macros
#define __LABELLED_PRINT(LABEL, FORMAT, ARGS...) std::println("{}" FORMAT "{}", LABEL, ARGS, Color::Reset);
#define __LABELLED_PRINT_NO_ARGS(LABEL, FORMAT) __LABELLED_PRINT(LABEL, FORMAT, "")

#define PRINT_ERROR(FORMAT, ARGS...)  __LABELLED_PRINT(ERROR, FORMAT, ARGS)
#define PRINTERROR(FORMAT) __LABELLED_PRINT_NO_ARGS(ERROR, FORMAT)
#define PRINT_WARNING(FORMAT, ARGS...) __LABELLED_PRINT(WARN,  FORMAT, ARGS)
#define PRINTWARNING(FORMAT) __LABELLED_PRINT_NO_ARGS(WARN, FORMAT)

#ifdef DEBUGGING // PRINT_DEBUG should do nothing in the Release build
    #define PRINT_DEBUG(FORMAT, ARGS...) __LABELLED_PRINT(DEBUG, FORMAT, ARGS)
    #define PRINTDEBUG(FORMAT) __LABELLED_PRINT_NO_ARGS(DEBUG, FORMAT)
#else
    #define PRINT_DEBUG(FORMAT, ARGS...) {}
    #define PRINTDEBUG(FORMAT) {}
#endif // DEBUGGING

#endif // PRINTING_H
