#ifndef PRINTING_H
#define PRINTING_H

#include <string>
#include <print> // IWYU pragma: keep // Include this header because if you're including the "printing" header, then you probably wanna print stuff, huh?

struct Color
{
    constexpr static const char* Reset   = "\x1b[0m";
    constexpr static const char* Black   = "30";
    constexpr static const char* Red     = "31";
    constexpr static const char* Green   = "32";
    constexpr static const char* Yellow  = "33";
    constexpr static const char* Blue    = "34";
    constexpr static const char* Magenta = "35";
    constexpr static const char* Cyan    = "36";
    constexpr static const char* White   = "37";
    constexpr static const char* Default = "39";
};

constexpr std::string COLOR(const std::string& color)
{ return "\x1b[" + color + "m"; }

constexpr std::string COLOR_BOLD(const std::string& color)
{ return "\x1b[1;" + color + "m"; }

constexpr std::string ERROR() { return COLOR_BOLD(Color::Red)     + "[ERROR]";   }
constexpr std::string WARN()  { return COLOR_BOLD(Color::Yellow)  + "[WARNING]"; }
constexpr std::string DEBUG() { return COLOR_BOLD(Color::Cyan)    + "[DEBUG]";   }

// Useful macros
#define __LABELLED_PRINT(LABEL, FORMAT, ARGS...) std::print("%s " FORMAT "%s\n", LABEL, ARGS, Color::Reset)
#define __LABELLED_PRINT_NO_ARGS(LABEL, FORMAT, ARGS...) std::print("%s " FORMAT "%s\n", LABEL, Color::Reset)

#define PRINT_ERROR(FORMAT, ARGS...)   __LABELLED_PRINT(ERROR(), FORMAT, ARGS)
#define PRINTERROR(FORMAT)   __LABELLED_PRINT_NO_ARGS(ERROR(), FORMAT)
#define PRINT_WARNING(FORMAT, ARGS...) __LABELLED_PRINT(WARN(),  FORMAT, ARGS)
#define PRINTWARNING(FORMAT) __LABELLED_PRINT_NO_ARGS(WARN(), FORMAT)

#ifdef DEBUGGING // PRINT_DEBUG should do nothing in the Release build
    #define PRINT_DEBUG(FORMAT, ARGS...) __LABELLED_PRINT(DEBUG(), FORMAT, ARGS)
    #define PRINTDEBUG(FORMAT) __LABELLED_PRINT_NO_ARGS(DEBUG(), FORMAT)
#else
    #define PRINT_DEBUG(FORMAT, ARGS...)
    #define PRINTDEBUG(FORMAT)
#endif // DEBUGGING

#endif // PRINTING_H
